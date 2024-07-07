using _Project._200_Dev.Entities.Player.Interfaces;
using _Project._200_Dev.Entities.Player.Stats;
using _Project._200_Dev.Networking.IL_Weaving.Server;
using UnityEngine;

namespace _Project._200_Dev.Entities.Player.Effects
{
    public class HealEffect : Effect
    {
        public override EffectType Type => EffectType.Good;
        protected override bool AddToEffectableList => false;
       
        public int HealAmount;


        [Server]
        protected override bool TryApply_Internal(IEffectable effectable, PlayerRefs applier, Vector3 applyPosition)
        {
            var entity = effectable.AffectedEntity;
            
            entity.Heal(HealAmount);
            return true;
        }

        protected override void KillEffect_Internal() { }
        
        public override Effect GetInstance()
        {
            return this;
        }

        public override float GetEffectValue()
        {
            return HealAmount;
        }
    }
}