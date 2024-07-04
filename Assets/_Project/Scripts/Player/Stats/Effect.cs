using System.Linq;
using UnityEngine;

namespace Project
{
    public enum EffectType
    {
        Good,
        Bad,
        Neutral
    }
    
    [System.Serializable]
    public abstract class Effect
    {
        public abstract EffectType Type { get; }
        
        protected IEffectable AffectedEffectable { get; private set; }
        
        protected abstract bool AddToEffectableList { get; }
        
        protected virtual bool OnlyApplyOnce => false;
        
        public bool TryApply(IEffectable effectable, PlayerRefs applier, Vector3 applyPosition)
        {
            if (OnlyApplyOnce)
            {
                if (effectable.AppliedEffects.Any(effect => effect.GetType() == GetType()))
                    return false;
            }
            
            AffectedEffectable = effectable;
            if (AddToEffectableList) AddToEffectable();

            bool applied = TryApply_Internal(effectable, applier, applyPosition);
            if (!applied && AddToEffectableList) RemoveFromEffectable();
            
            return applied;
        }

        protected abstract bool TryApply_Internal(IEffectable effectable, PlayerRefs applier, Vector3 applyPosition);
        
        public void KillEffect()
        {
            if (AddToEffectableList) RemoveFromEffectable();
            
            KillEffect_Internal();
        }

        protected abstract void KillEffect_Internal();

        private void AddToEffectable()
        {
            AffectedEffectable.SrvAddEffect(this);
        }

        private void RemoveFromEffectable()
        {
            AffectedEffectable.SrvRemoveEffect(this);
        }

        public abstract Effect GetInstance();

        public virtual float GetEffectValue() {  return 0; }

        public virtual float GetEffectDuration() { return 0; }
    }
}