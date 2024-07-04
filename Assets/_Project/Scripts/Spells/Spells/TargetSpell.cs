using System;
using System.Collections.Generic;
using Project._Project.Scripts;
using Project._Project.Scripts.Managers;
using Sirenix.OdinInspector;
using Unity.Netcode;
using UnityEngine;

namespace Project.Spells
{
    public class TargetSpell : Spell
    {
        [SerializeField] private SpawnType spawnType;
        [SerializeField] private MoveType moveType;

        [SerializeField] private float moveSpeed;

        [SerializeField] private RotationType rotationType;
        
        [SerializeField] private DieType dieType;

        [SerializeField, ShowIf(nameof(IsTimed))] private float aliveTime;
        
        [SerializeField] private ApplyEffectType applyEffectType;
        
        private NetworkObject _target;
        
        private Vector3 _lastPosition;
        
        protected override void Init(ICastResult castResult)
        {
            _target = GetTarget(castResult);
            
            if (!_target)
            {
                Debug.LogError("TargetSpell could not find target!");
                KillSpell();
                return;
            }
            
            if (applyEffectType == ApplyEffectType.OnSpawn)
                ApplyEffectsOnTarget();
        }

        private void Update()
        {
            if (!IsOnServer)
            {
                ClientUpdate();
                return;
            }
            
            HandleMovement(_target.transform);
            HandleDeath();
        }

        private void ClientUpdate()
        {
            var diff = transform.position - _lastPosition;
            if (diff == Vector3.zero) return;
            
            HandleRotation(diff);
                
            _lastPosition = transform.position;
        }
        
        private void HandleMovement(Transform target)
        {
            Vector3 direction;
            
            switch (moveType)
            {
                case MoveType.None:
                    direction = default;
                    break;
                case MoveType.ToTarget:
                    direction = target.position - transform.position;
                    break;
                case MoveType.ToCaster:
                    direction = Caster.PlayerTransform.position - transform.position;;
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
                
            direction.y = 0;
            direction.Normalize();
            
            transform.position += direction * (moveSpeed * Time.deltaTime);
            HandleRotation(direction);
        }

        private void HandleRotation(Vector3 direction)
        {
            switch (rotationType)
            {
                case RotationType.None:
                    break;
                case RotationType.Movement:
                    transform.rotation = Quaternion.LookRotation(direction);
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        private void HandleDeath()
        {
            switch(dieType)
            {
                case DieType.OnImpact:
                    CheckForImpact();
                    break;
                case DieType.OnTime:
                    CheckForTime();
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            return;

            void CheckForImpact()
            {
                Transform impactTarget;
                
                switch (moveType)
                {
                    case MoveType.None:
                        return;  
                    case MoveType.ToTarget:
                        impactTarget = _target.transform;
                        break;
                    case MoveType.ToCaster:
                        impactTarget = Caster.PlayerTransform;
                        break;
                    default:
                        throw new ArgumentOutOfRangeException();
                }
                
                var spellPosition = transform.position;
                spellPosition.y = 0;
                
                var impactPosition = impactTarget.position;
                impactPosition.y = 0;
                
                if (Vector3.Distance(spellPosition, impactPosition) < moveSpeed * Time.deltaTime + 0.1f)
                    KillSpell();
            }

            void CheckForTime()
            {
                aliveTime -= Time.deltaTime;
                if (aliveTime <= 0)
                    KillSpell();
            }
        }

        private void ApplyEffectsOnTarget()
        {
            if (_target.TryGetComponent(out Entity entity))
                TryApplyEffects(entity);
            else
                Debug.LogError($"Designed target for TargetSpell {gameObject.name} does not have an Entity component!");
        }
        
        protected override void KillSpell()
        {
            if (applyEffectType == ApplyEffectType.OnKill)
                ApplyEffectsOnTarget();
            
            base.KillSpell();
        }
        
        public override void OnNetworkSpawn()
        {
            _lastPosition = transform.position;
            SoundManager.instance.PlaySingleSound("inst_" + Data.spellId, gameObject, SoundManager.EventType.Spell);
        }

        public override (Vector3, Quaternion) GetDefaultTransform(ICastResult castResult, PlayerRefs player)
        {
            var target = GetTarget(castResult);
            if (!target) return default;

            var rotation = Quaternion.LookRotation(GetDirection(target.transform, player));
            
            return spawnType switch
            {
                SpawnType.OnTarget => new(target.transform.position, rotation),
                SpawnType.OnCaster => new(player.ShootTransform.position, rotation),
                _ => throw new ArgumentOutOfRangeException()
            };
        }

        public override Vector3 GetDirection(ICastResult castResult, PlayerRefs player)
        {
            var target = GetTarget(castResult);
            if (!target) return default;
            
            return GetDirection(target.transform, player);
        }

        private Vector3 GetDirection(Transform target, PlayerRefs player)
        {
            var directionToTarget = target.position - player.PlayerTransform.position;
            directionToTarget.y = 0;
            directionToTarget.Normalize();
            
            return moveType switch
            {
                MoveType.None => directionToTarget,
                MoveType.ToTarget => directionToTarget,
                MoveType.ToCaster => -directionToTarget,
                _ => throw new ArgumentOutOfRangeException()
            };
        }

        private NetworkObject GetTarget(ICastResult castResult)
        {
            if (castResult is not IntResults results)
            {
                Debug.LogError(
                    $"Given channeling result {nameof(castResult)} is not the required type for {nameof(TargetSpell)}!");
                return null;
            }
            
            return NetworkManager.Singleton.SpawnManager.SpawnedObjects.GetValueOrDefault((ulong)results.IntProp);
        }
        
        private enum SpawnType
        {
            OnTarget,
            OnCaster
        }
        
        private enum MoveType
        {
            None,
            ToTarget,
            ToCaster
        }
        
        private enum RotationType
        {
            None,
            Movement
        }

        private enum DieType
        {
            OnImpact,
            OnTime
        }

        private enum ApplyEffectType
        {
            OnKill,
            OnSpawn
        }
        
        private bool IsTimed()
        {
            return dieType == DieType.OnTime;
        }
    }
}