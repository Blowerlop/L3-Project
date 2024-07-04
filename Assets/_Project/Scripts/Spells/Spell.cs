using System.Linq;
using Project._Project.Scripts;
using Sirenix.OdinInspector;
using Unity.Netcode;
using UnityEngine;

namespace Project.Spells
{
    public abstract class Spell : NetworkBehaviour
    {
        protected PlayerRefs Caster { get; private set; }
        protected int CasterTeamIndex => Caster.TeamIndex;
        
        protected bool IsServerOnly { get; private set; }
        
        // Must use this instead of NetworkBehaviour.IsServer to account for server only spells
        protected bool IsOnServer => IsServerOnly || IsServer;
            
        [field: SerializeField, ReadOnly] public SpellData Data { get; set; }
        
        [SerializeField] protected bool CanHitSelf;
        
        // Called by Server
        // Used to set field that are common for every spell before calling overriden Init
        public void Init(ICastResult castResult, PlayerRefs player, bool serverOnly)
        {
            Caster = player;
            IsServerOnly = serverOnly;
            
            ApplyOnCasterEffects();
            
            Init(castResult);
        }
        
        protected abstract void Init(ICastResult castResult);

        public abstract (Vector3, Quaternion) GetDefaultTransform(ICastResult castResult, PlayerRefs player);
        
        public abstract Vector3 GetDirection(ICastResult castResult, PlayerRefs player);

        protected virtual bool TryApplyEffects(Entity entity)
        {
            if (!CanHitSelf && Caster.GetPC().Entity == entity)
                return false;
            
            int appliedEffects = Data.effects.Count(effect => effect.GetInstance().TryApply(entity, applier: Caster, applyPosition: transform.position));
            return appliedEffects > 0;
        }

        private void ApplyOnCasterEffects()
        {
            var entity = Caster.GetPC().Entity;
            
            foreach (var onCasterEffect in Data.onCasterEffects)
            {
                onCasterEffect.GetInstance().TryApply(entity, applier: Caster, applyPosition: transform.position);
            }
        }

        protected virtual void KillSpell()
        {
            if (IsServerOnly) Destroy(gameObject);
            else NetworkObject.Despawn();
        }
    }
}