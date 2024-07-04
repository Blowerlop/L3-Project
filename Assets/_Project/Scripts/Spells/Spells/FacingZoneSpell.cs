using System.Collections.Generic;
using Project._Project.Scripts;
using Project._Project.Scripts.Managers;
using UnityEngine;
using Debug = UnityEngine.Debug;

namespace Project.Spells
{
    public class FacingZoneSpell : Spell
    {
        [SerializeField] private float duration;

        [SerializeField] private BoxCollider collidersParent;
        private BoxCollider[] _colliders;
        
        private SingleVectorResults _results;

        private void Awake()
        {
            _colliders = collidersParent.GetComponentsInChildren<BoxCollider>();
        }

        protected override void Init(ICastResult castResult)
        {
            if(castResult is not SingleVectorResults results)
            {
                Debug.LogError($"Given channeling result {nameof(castResult)} is not the required type for {nameof(FacingZoneSpell)}!");
                return;
            }
            
            _results = results;
            
            CheckForEffects();
            
            StartCoroutine(Utilities.WaitForSecondsAndDoActionCoroutine(duration, KillSpell));
        }

        private void CheckForEffects()
        {
            List<Entity> hits = new List<Entity>();
            Collider[] results = new Collider[5];
            
            foreach (var col in _colliders)
            {
                var pos = col.transform.rotation * col.center + col.transform.position;
                
                var size = Physics.OverlapBoxNonAlloc(pos, col.size / 2, results,
                    transform.rotation, Constants.Layers.EntityMask);
                
                for(int i = 0; i < size; i++)
                {
                    if(results[i].TryGetComponent(out Entity entity))
                    {
                        if (!hits.Contains(entity))
                            hits.Add(entity);
                    }
                }
            }

            foreach (var hit in hits)
            {
                TryApplyEffects(hit);
            }
        }

        public override (Vector3, Quaternion) GetDefaultTransform(ICastResult castResult, PlayerRefs player)
        {
            if(castResult is not SingleVectorResults results)
            {
                Debug.LogError($"Given channeling result {nameof(castResult)} is not the required type for {nameof(FacingZoneSpell)}!");
                return default;
            }
            
            var dir = GetDirection(castResult, player);
            
            return (player.PlayerTransform.position, Quaternion.LookRotation(dir));
        }

        public override Vector3 GetDirection(ICastResult castResult, PlayerRefs player)
        {
            if (castResult is not SingleVectorResults results)
            {
                Debug.LogError($"Given channeling result {nameof(castResult)} is not the required type for {nameof(FacingZoneSpell)}!");
                return default;
            }
            
            var dir = results.VectorProp - player.PlayerTransform.position;
            dir.y = 0;
            dir.Normalize();
            
            return dir;
        }
        
        public override void OnNetworkSpawn()
        {
            SoundManager.instance.PlaySingleSound("inst_" + Data.spellId, gameObject, SoundManager.EventType.Spell);
        }
    }
}