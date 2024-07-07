using System.Collections.Generic;
using _Project._200_Dev.Entities.Player.Interfaces;
using _Project._200_Dev.Entities.Player.Stats;
using UnityEngine;

namespace _Project._200_Dev.Entities.Player.Effects
{
    public class NextAutoEffect : Effect
    {
        public override EffectType Type => EffectType.Neutral;
        protected override bool AddToEffectableList => true;
        protected override bool OnlyApplyOnce => true;

        [SerializeReference, SerializeField] private List<Effect> _effectsOnAuto = new();

        protected override bool TryApply_Internal(IEffectable effectable, PlayerRefs applier, Vector3 applyPosition)
        {
            return true;
        }

        protected override void KillEffect_Internal() { }

        public bool TryApplyChildEffects(IEffectable effectable, PlayerRefs applier)
        {
            int appliedEffects = 0;
            
            foreach (var effect in _effectsOnAuto)
            {
                if (effect.GetInstance().TryApply(effectable, applier, default))
                    appliedEffects++;
            }

            return appliedEffects > 0;
        }

        public override Effect GetInstance()
        {
            return new NextAutoEffect() { _effectsOnAuto = _effectsOnAuto };
        }
    }
}