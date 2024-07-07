using System.Collections.Generic;
using Project._Project.Scripts;
using Project._Project.Scripts.Managers;
using UnityEngine;

namespace Project.Spells
{
    public class BoomerangSpell : Spell
    {
        [SerializeField] private float moveSpeed;
        [SerializeField] private float firstHalfDuration, secondHalfEaseDuration;

        [SerializeField] private AnimationCurve firstHalfEase, secondHalfEase;

        [SerializeField] private BoxCollider hitCollider;

        private List<Entity> _hitEntities = new List<Entity>();
        
        private SingleVectorResults _results;
        private Vector3 _castDir;
        
        private float _timer;
        private bool _isOnFirstHalf;
        
        protected override void Init(ICastResult castResult)
        {
            if (castResult is not SingleVectorResults results)
            {
                Debug.LogError(
                    $"Given channeling result {nameof(castResult)} is not the required type for {nameof(BoomerangSpell)}!");
                return;
            }
            
            _results = results;
            _castDir = GetDirection(castResult, Caster);
            
            _isOnFirstHalf = true;
        }

        private void Update()
        {
            if (!IsOnServer) return;
            
            _timer += Time.deltaTime;
            
            if (_isOnFirstHalf) FirstHalf();
            else SecondHalf();
            
            CheckForEffects();
        }

        [Server]
        private void FirstHalf()
        {
            _timer += Time.deltaTime;
            
            var easeFactor = firstHalfEase.Evaluate(_timer / firstHalfDuration);
            transform.position += _castDir * (moveSpeed * easeFactor * Time.deltaTime);
            
            if (_timer >= firstHalfDuration) 
                SwitchToSecondHalf();
        }
        
        [Server]
        private void SwitchToSecondHalf()
        {
            _timer = 0;
            _hitEntities.Clear();
            
            _isOnFirstHalf = false;
        }
        
        [Server]
        private void SecondHalf()
        {
            var dir = Caster.PlayerTransform.position - transform.position;
            dir.y = 0;
                
            var normalizedDir = dir.normalized;

            var easeFactor = secondHalfEase.Evaluate((_timer) / secondHalfEaseDuration);

            var speed = moveSpeed * easeFactor * Time.deltaTime;
            
            transform.position += normalizedDir * speed;
                
            if(dir.magnitude < speed + 0.1f) 
                KillSpell();
        }
        
        [Server]
        private void CheckForEffects()
        {
            var results = new Collider[5];

            var size = Physics.OverlapBoxNonAlloc(transform.position, hitCollider.bounds.extents, results, hitCollider.transform.rotation, Constants.Layers.EntityMask);

            for(int i = 0; i < size; i++)
            {
                if (!results[i].TryGetComponent(out Entity entity) || _hitEntities.Contains(entity)) continue;
                
                if (TryApplyEffects(entity)) 
                    _hitEntities.Add(entity);
            }
        }
        
        public override (Vector3, Quaternion) GetDefaultTransform(ICastResult castResult, PlayerRefs player)
        {
            if (castResult is not SingleVectorResults results)
            {
                Debug.LogError(
                    $"Given channeling result {nameof(castResult)} is not the required type for {nameof(BoomerangSpell)}!");
                return default;
            }
            
            return (player.ShootTransform.position, Quaternion.LookRotation(GetDirection(castResult, player)));
        }

        public override Vector3 GetDirection(ICastResult castResult, PlayerRefs player)
        {
            if (castResult is not SingleVectorResults results)
            {
                Debug.LogError(
                    $"Given channeling result {nameof(castResult)} is not the required type for {nameof(BoomerangSpell)}!");
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