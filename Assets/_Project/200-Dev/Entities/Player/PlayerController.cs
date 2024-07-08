using _Project._200_Dev.Entities.Player.States;
using _Project._200_Dev.Entities.Player.Stats;
using _Project._200_Dev.Entities.StateMachine;
using _Project._200_Dev.Spells;
using _Project._200_Dev.Utilities.Extensions;
using Cinemachine;
using Project;
using Sirenix.OdinInspector;
using Unity.Netcode;
using UnityEngine;

namespace _Project._200_Dev.Entities.Player
{
    public class PlayerController : Entity
    {
        [SerializeField] private PCPlayerRefs _refs;

        private int _currentAnimationHash;
        [ShowInInspector] private NetworkVariable<int> _currentAnimation = new();
        
        [SerializeField] private CinemachineVirtualCamera _deathCamera;
        

        public override int TeamIndex => _refs.TeamIndex;

        private void Start()
        {
            SpellManager.OnChannelingStarted += OnChannelingStarted;
        }
        

        public override void OnDestroy()
        {
            base.OnDestroy();
            
            SpellManager.OnChannelingStarted -= OnChannelingStarted;
        }
        
        private void OnChannelingStarted(PlayerRefs player, Vector3 direction)
        {
            if (player != _refs) return;
            
            _refs.PlayerTransform.rotation = Quaternion.LookRotation(direction);
        }

        public override void OnNetworkSpawn()
        {
            base.OnNetworkSpawn();
            
            if (IsServer)
            {
                if (_stats.isInitialized)
                {
                    OnStatsInitialized_HookHealth();
                }
                else _stats.OnStatsInitialized += OnStatsInitialized_HookHealth;
            }
            if (IsOwner)
            {
                _deathCamera = GameObject.FindGameObjectWithTag(Constants.Tags.Death_Camera)?.GetComponent<CinemachineVirtualCamera>();

                if (IsServer)
                {
                    _refs.StateMachine.SrvOnStateEnter += OwnerOnDeadStateEnter_EnableDeathCamera;
                    _refs.StateMachine.SrvOnStateExit += OwnerOnDeadStateExit_DisableDeathCamera;
                }
                else if (IsClient)
                {
                    _refs.StateMachine.CliOnStateEnter += OwnerOnDeadStateEnter_EnableDeathCamera;
                    _refs.StateMachine.CliOnStateExit += OwnerOnDeadStateExit_DisableDeathCamera;
                }
            }
        }

        public override void OnNetworkDespawn()
        {
            base.OnNetworkDespawn();
            
            if (IsServer) _stats.Get<HealthStat>().OnValueChanged -= OnHealthChanged_CheckIfDead;
            if (IsOwner)
            {
                if (IsServer)
                {
                    _refs.StateMachine.SrvOnStateEnter -= OwnerOnDeadStateEnter_EnableDeathCamera;
                    _refs.StateMachine.SrvOnStateExit -= OwnerOnDeadStateExit_DisableDeathCamera;
                }
                else if (this.IsClientOnly())
                {
                    _refs.StateMachine.CliOnStateEnter -=  OwnerOnDeadStateEnter_EnableDeathCamera;
                    _refs.StateMachine.CliOnStateExit -= OwnerOnDeadStateExit_DisableDeathCamera;
                }
            }
        }

        protected override void OnStunned()
        {
            base.OnStunned();

            if (_refs.StateMachine.currentState is DeadState) return;
            
            _refs.StateMachine.ChangeStateTo<StunState>();
        }

        protected override void OnUnStunned()
        {
            base.OnUnStunned();
            
            if (_refs.StateMachine.currentState is DeadState) return;
            
            _refs.StateMachine.ChangeStateTo<IdleState>();
        }

        private void OnHealthChanged_CheckIfDead(int currentHealth, int maxHealth)
        {
            if (currentHealth <= 0)
            {
                _refs.StateMachine.ChangeStateTo<DeadState>();
            }
        }

        private void OnStatsInitialized_HookHealth()
        {
            _stats.Get<HealthStat>().OnValueChanged += OnHealthChanged_CheckIfDead;
            _stats.OnStatsInitialized -= OnStatsInitialized_HookHealth;
        }
        
        private void OwnerOnDeadStateEnter_EnableDeathCamera(BaseStateMachineBehaviour currentState)
        {
            if (currentState is DeadState)
            {
                Debug.Log("Enable death camera");
                _deathCamera.enabled = true;
            }
        }
        
        private void OwnerOnDeadStateExit_DisableDeathCamera(BaseStateMachineBehaviour currentState)
        {
            if (currentState is DeadState)
            {
                Debug.LogError(_refs.PlayerTransform.position);
                _deathCamera.enabled = false;
            }
        }
    }
}
