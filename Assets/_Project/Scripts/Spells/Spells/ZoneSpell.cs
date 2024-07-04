using System;
using DG.Tweening;
using Project._Project.Scripts;
using Project._Project.Scripts.Managers;
using Sirenix.OdinInspector;
using UnityEngine;

namespace Project.Spells
{
    public class ZoneSpell : Spell
    {
        private ZoneSpellData _zoneData = null;
        private ZoneSpellData ZoneData => _zoneData ??= Data as ZoneSpellData;
        
        [SerializeField] private ApplyType applyType;
        [SerializeField, ShowIf(nameof(IsApplyTimed))] private float applyTime;
        [SerializeField, ShowIf(nameof(IsApplyTick))] private float tickRate;
        
        [SerializeField] private KillType killType;

        [SerializeField, ShowIf(nameof(IsTimed))] private float duration;
        
        [SerializeField] private HitZoneShape hitZoneShape;
        
        [SerializeField, ShowIf(nameof(IsBoxShape))] private BoxCollider boxCollider;
        
        private SingleVectorResults _results;
        
        private Sequence _moveSeq;
        private float _lastTickTime;

        private Collider[] _resultsBuffer = new Collider[5];

        private Tween _timedTween;
        
        protected override void Init(ICastResult castResult)
        {
            if (castResult is not SingleVectorResults results)
            {
                Debug.LogError(
                    $"Given channeling result {nameof(castResult)} is not the required type for {nameof(ZoneSpell)}!");
                return;
            }
            
            _results = results;
            
            switch (applyType)
            {
                case ApplyType.OnStart:
                    CheckForEffects();
                    break;
                case ApplyType.Timed:
                    _timedTween = DOVirtual.DelayedCall(applyTime, CheckForEffects);
                    break;
            }
            
            if (killType == KillType.Timed)
                StartCoroutine(Utilities.WaitForSecondsAndDoActionCoroutine(duration, KillSpell));
        }

        public override void OnDestroy()
        {
            base.OnDestroy();
            
            if (!IsOnServer) return;
            if (IsApplyTimed()) return;
            
            if (_timedTween.IsActive())
            {
                _timedTween.Kill();
            }
        }
        
        public override (Vector3, Quaternion) GetDefaultTransform(ICastResult castResult, PlayerRefs player)
        {
            if (castResult is not SingleVectorResults results)
            {
                Debug.LogError(
                    $"Given channeling result {nameof(castResult)} is not the required type for {nameof(ZoneSpell)}!");
                return default;
            }

            Quaternion rotation = Quaternion.identity;

            if (ZoneData.lookAtCenter)
                rotation = Quaternion.LookRotation(GetDirection(castResult, player));
            
            return (results.VectorProp, rotation);
        }

        public override Vector3 GetDirection(ICastResult castResult, PlayerRefs player)
        {
            if (castResult is not SingleVectorResults results)
            {
                Debug.LogError(
                    $"Given channeling result {nameof(castResult)} is not the required type for {nameof(ZoneSpell)}!");
                return default;
            }
            
            var dir = results.VectorProp - player.PlayerTransform.position;
            dir.y = 0;
            dir.Normalize();
            
            return dir;
        }

        private void Update()
        {
            if (!IsServer) return;
            if (!IsApplyTick()) return;
            
            if (Time.time - _lastTickTime >= tickRate)
            {
                CheckForEffects();
                _lastTickTime = Time.time;
            }
        }
        
        [Server]
        private void CheckForEffects()
        {
            var size = hitZoneShape switch
            {
                HitZoneShape.Default => Physics.OverlapSphereNonAlloc(_results.VectorProp, ZoneData.zoneRadius, _resultsBuffer,
                    Constants.Layers.EntityMask),
                HitZoneShape.Box => Physics.OverlapBoxNonAlloc(transform.position + boxCollider.center,
                    boxCollider.size / 2, _resultsBuffer, boxCollider.transform.rotation, Constants.Layers.EntityMask),
                _ => throw new ArgumentOutOfRangeException()
            };

            for(int i = 0; i < size; i++)
            {
                if(_resultsBuffer[i].TryGetComponent(out Entity entity))
                {
                    TryApplyEffects(entity);
                }
            }
        }

        [Server]
        public void AnimEnd()
        {
            KillSpell();
        }
        
        public override void OnNetworkSpawn()
        {
            base.OnNetworkSpawn();
            
            SoundManager.instance.PlaySingleSound("inst_" + Data.spellId, gameObject, SoundManager.EventType.Spell);
        }
        
        private enum ApplyType
        {
            AnimationDriven,
            OnStart,
            Timed,
            Tick
        }
        
        private enum KillType
        {
            AnimationDriven,
            Timed
        }

        private enum HitZoneShape
        {
            Default,
            Box
        }
        
        private bool IsApplyTimed() => applyType == ApplyType.Timed;
        private bool IsApplyTick() => applyType == ApplyType.Tick;
        private bool IsTimed() => killType == KillType.Timed;
        private bool IsBoxShape() => hitZoneShape == HitZoneShape.Box;
    }
}
