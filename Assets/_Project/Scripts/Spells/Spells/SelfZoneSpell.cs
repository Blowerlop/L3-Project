using Project._Project.Scripts;
using Project._Project.Scripts.Managers;
using Sirenix.OdinInspector;
using UnityEngine;

namespace Project.Spells
{
    public class SelfZoneSpell : Spell
    {
        [SerializeField] private bool followPlayer;
        [SerializeField] private float duration = 2f;

        [SerializeField] private float zoneRadius;
        
        [SerializeField] private ApplyType applyType;
        [SerializeField, ShowIf(nameof(IsTimedApplyType))] private float timeBetweenApplies;
        
        private readonly Collider[] _hits = new Collider[10];
        private float _applyTimer;
        
        protected override void Init(ICastResult castResult)
        {
            if (castResult is not EmptyResults)
            {
                Debug.LogError(
                    $"Given channeling result {nameof(castResult)} is not the required type for {nameof(SelfTargetSpell)}!");
                return;
            }
            
            if (applyType == ApplyType.OnStart)
                CheckForEffects();

            StartCoroutine(Utilities.WaitForSecondsAndDoActionCoroutine(duration, KillSpell));
        }
        
        protected override void KillSpell()
        {
            if (applyType == ApplyType.OnKill)
                CheckForEffects();
            
            base.KillSpell();
        }

        #region Effects
        
        private void CheckForEffects()
        {
            var size = Physics.OverlapSphereNonAlloc(transform.position, zoneRadius, _hits, Constants.Layers.EntityMask);
            if (size == 0) return;
            
            for (int i = 0; i < size; i++)
            {
                if (_hits[i].TryGetComponent(out Entity entity))
                {
                    TryApplyEffects(entity);
                }
            }
        }
        
        #endregion
        
        #region Update

        private void Update()
        {
            if (!IsOnServer) return;
            if (!IsTimedApplyType()) return;
            
            _applyTimer -= Time.deltaTime;
            
            if (_applyTimer <= 0)
            {
                CheckForEffects();
                _applyTimer = timeBetweenApplies;
            }
        }

        private void LateUpdate()
        {
            if (!IsOnServer) return;
            if (!followPlayer) return;
            
            transform.position = Caster.PlayerTransform.position;
        }

        #endregion
        
        #region Audio
        
        public override void OnNetworkSpawn()
        {
            base.OnNetworkSpawn();
            
            SoundManager.instance.PlaySingleSound("inst_" + Data.spellId, gameObject, SoundManager.EventType.Spell);
        }
        
        #endregion
        
        #region Utils
        
        public override (Vector3, Quaternion) GetDefaultTransform(ICastResult castResult, PlayerRefs player)
        {
            if (castResult is not EmptyResults)
            {
                Debug.LogError(
                    $"Given channeling result {nameof(castResult)} is not the required type for {nameof(SelfTargetSpell)}!");
                return default;
            }
            
            return (player.PlayerTransform.position, Quaternion.identity);
        }

        public override Vector3 GetDirection(ICastResult castResult, PlayerRefs player)
        {
            if (castResult is not EmptyResults)
            {
                Debug.LogError(
                    $"Given channeling result {nameof(castResult)} is not the required type for {nameof(SelfTargetSpell)}!");
                return default;
            }
            
            return player.PlayerTransform.forward;
        }
        
        private enum ApplyType
        {
            OnStart,
            OnKill,
            Timed
        }

        private bool IsTimedApplyType()
        {
            return applyType == ApplyType.Timed;
        }
        
        #endregion
    }
}
