using System.Collections;
using Project._Project.Scripts;
using Project._Project.Scripts.Player.States;
using Project._Project.Scripts.StateMachine;
using Project.Effects;
using Project.Extensions;
using Unity.Netcode;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Project
{
    public class AttackController : NetworkBehaviour
    {
        [SerializeField] private float attackingRangeOffset = 2f;
        
        private PCPlayerRefs _playerRefs;
        private Camera _camera;
        public NetworkObject targetNetworkObject;
        private IEffectable _effectable;
        private bool _isRanged;
        private SOProjectile _projectileData;
        
        private DamageEffect _cachedDamageEffect;

        private bool _attackEngaged;
        private bool _isAttacking;
        
        private void Awake()
        {
            _playerRefs = GetComponentInParent<PCPlayerRefs>();
        }

        public override void OnNetworkSpawn()
        {
            if (IsOwner)
            {
                _playerRefs.PlayerMouse.OnMouseClick += OnMouseButton1_AttackRequest;
                InputManager.instance.onCancellation.performed += OnCancellation_StopAttack;
            }

            if (IsServer)
            {
                _playerRefs.Entity.onEntityInit += OnEntityInit;
                _playerRefs.StateMachine.SrvOnStateExit += SrvOnStateExitEndAttack;
            }
        }

        public override void OnNetworkDespawn()
        {
            base.OnNetworkDespawn();

            if (IsOwner)
            {
                _playerRefs.PlayerMouse.OnMouseClick -= OnMouseButton1_AttackRequest;
                if (InputManager.IsInstanceAlive())
                    InputManager.instance.onCancellation.performed -= OnCancellation_StopAttack;
            }

            if (IsServer)
            {
                _playerRefs.Entity.onEntityInit -= OnEntityInit;
                _playerRefs.StateMachine.SrvOnStateExit -= SrvOnStateExitEndAttack;
            }
        }

        private void FixedUpdate()
        {
            if (IsServer && targetNetworkObject != null)
            {
                if (_attackEngaged) return;
                
                var pos = targetNetworkObject.transform.position;
                if (IsInRange(pos))
                {
                    _playerRefs.PlayerTransform.rotation =
                        Quaternion.LookRotation((pos - _playerRefs.PlayerTransform.position).ResetAxis(EAxis.Y)
                            .normalized);
                    SrvTryToAttack(targetNetworkObject);
                }
                else GoToContact(targetNetworkObject.transform);
            }
        }

        private void OnEntityInit()
        {
            SOCharacter characterData = (SOCharacter)_playerRefs.Entity.data;
            _isRanged = characterData.attackType == EAttackType.Ranged;
            if (_isRanged) _projectileData = characterData.projectileData;
        }

        // [Owner]
        private void OnMouseButton1_AttackRequest(RaycastHit hitInfo, int layer)
        {
            if (!Gameloop.IsGameRunning) return;
            
            if (layer != Constants.Layers.EntityIndex)
            {
                if (targetNetworkObject != null)
                {
                    targetNetworkObject = null;
                    RemoveTargetServerRpc();
                }

                return;
            }

            // Now using effectable to apply effects. But still checks with Damageable to get IsDamageable
            if (!IsDamageable(hitInfo.transform, out IDamageable _)) return;

            targetNetworkObject = hitInfo.transform.GetComponentInParent<NetworkObject>();
            if (targetNetworkObject == null)
            {
                Debug.LogError("Target to attack is not a network object");
                return;
            }

            TryToAttackServerRpc(targetNetworkObject);
        }

        [ServerRpc]
        private void TryToAttackServerRpc(NetworkObjectReference networkObjectReference)
        {
            SrvTryToAttack(networkObjectReference);
        }

        [Server]
        private void SrvTryToAttack(NetworkObjectReference networkObjectReference)
        {
            if (!Gameloop.IsGameRunning) return;
            
            networkObjectReference.TryGet(out targetNetworkObject);
            if (targetNetworkObject == null)
            {
                Debug.LogError("Not NetworkObject found for the networkObjectReference id " +
                               networkObjectReference.NetworkObjectId);
                return;
            }

            if (IsAttacking())
            {
                // Use effectable to make it work even on host.
                // targetNetworkObject is set before RPC in OnMouseButton1_AttackRequest, so host have already the new one before this.
                if(_effectable.AffectedEntity.NetworkObjectId == targetNetworkObject.NetworkObjectId) return;
            }

            if (IsInRange(targetNetworkObject.transform.position) == false) return;
            
            // If we are here while attacking that means we have changed target
            if (_playerRefs.StateMachine.CanChangeStateTo<AttackState>() || IsAttacking())
            {
                StartAttack(targetNetworkObject.GetComponentInChildren<IEffectable>());
            }
        }

        [Server]
        private void StartAttack(IEffectable effectable)
        {
            _isAttacking = true;
            _playerRefs.NetworkAnimator.Animator.SetFloat(Constants.AnimatorsParam.AttackSpeed, _playerRefs.Entity.Stats.Get<AttackSpeedStat>().value);
            
            if (_playerRefs.StateMachine.currentState is not AttackState)
                _playerRefs.StateMachine.ChangeStateTo<AttackState>();

            _effectable = effectable;
        }

        // Ca ne se passe pas bien dans l'animator si on ne skip pas une frame :)
        [Server]
        private IEnumerator EndAttack()
        {
            yield return null;
            
            if (_playerRefs.StateMachine.currentState is AttackState)
            {
                _playerRefs.StateMachine.ChangeStateTo<IdleState>();
            }

            ResetAttack();
        }

        private void ResetAttack()
        {
            _isAttacking = false;
            _attackEngaged = false;
        }

        [Server]
        public void Hit(IEffectable effectable)
        {
            IEffectable ownEffectable = _playerRefs.Entity;
                
            for (int i = ownEffectable.AppliedEffects.Count - 1; i >= 0; i--)
            {
                var effect = ownEffectable.AppliedEffects[i];
                if (effect is not NextAutoEffect nextAutoEffect) continue;
                    
                nextAutoEffect.TryApplyChildEffects(effectable, _playerRefs);
                nextAutoEffect.KillEffect();
            }

            _cachedDamageEffect ??= new DamageEffect();
            _cachedDamageEffect.DamageAmount = _playerRefs.Entity.Stats.Get<AttackDamageStat>().value;
            
            _cachedDamageEffect.TryApply(effectable, _playerRefs, _playerRefs.PlayerTransform.position);    
            
            /*if (_isRanged == false) StartCoroutine(EndAttack());*/
        }

        // Called by animation event
        public void Hit()
        {
            if (IsServer == false) return;
            
            // We have lost the target during the attack because we wanted to cancel the attack but the animation transition made it to this point
            if (targetNetworkObject == null) return;

            if (_isRanged)
            {
                Projectile projectileInstance = Instantiate(_projectileData.prefab, _playerRefs.ShootTransform.position, _playerRefs.ShootTransform.rotation);
                projectileInstance.Init(this, _projectileData);
                projectileInstance.GetComponent<NetworkObject>().Spawn(true);

                /*StartCoroutine(EndAttack());*/
            }
            else
            {
                Hit(_effectable);
            }

        }
        
        private void OnCancellation_StopAttack(InputAction.CallbackContext _)
        {
            StopAttackServerRpc();
        }

        [ServerRpc]
        private void StopAttackServerRpc()
        {
            targetNetworkObject = null;
            StartCoroutine(EndAttack());
        }

        [Server]
        private void GoToContact(Transform target)
        {
            _playerRefs.MovementCOntroller.SrvGoTo(target.position);
        }

        [ServerRpc]
        private void RemoveTargetServerRpc()
        {
            targetNetworkObject = null;
        }

        public bool IsDamageable(Transform target, out IDamageable damageable)
        {
            return target.TryGetComponent(out damageable) && damageable.CanDamage(_playerRefs.TeamIndex);
        }

        [Server]
        private bool IsInRange(Vector3 targetPosition)
        {
            float attackRange = _playerRefs.Entity.Stats.Get<AttackRangeStat>().value;

            if (IsAttacking()) attackRange += attackingRangeOffset;
            
            return (targetPosition - _playerRefs.PlayerTransform.position).ResetAxis(EAxis.Y).sqrMagnitude <
                   attackRange * attackRange;
        }

        [Server]
        private bool IsAttacking()
        {
            return _isAttacking;
        }

        [Server]
        private void SrvOnStateExitEndAttack(BaseStateMachineBehaviour exitState)
        {
            // In case we want to move the moment we're attacking. Cancel the attack and move
            if (exitState is AttackState)
            {
                if (IsAttacking())
                {
                    ResetAttack();
                }
            }
        }

        [Server]
        public void SrvForceReset()
        {
            targetNetworkObject = null;
            ResetAttack();
        }

        public void SetAttackEngaged(bool value)
        {
            // Called by an anim event, we don't want to use [Server] because we would get errors on client
            if (!IsServer) return;
            if (!IsAttacking() && value) return;
            
            _attackEngaged = value;
        }
    }
}
