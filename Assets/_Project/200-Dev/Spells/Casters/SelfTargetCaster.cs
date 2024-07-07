using System;
using _Project._200_Dev.Spells.Results;

namespace _Project._200_Dev.Spells.Casters
{
    public class SelfTargetCaster : SpellCaster
    {
        public override Type CastResultType => typeof(EmptyResults);
        public override Type SpellDataType => typeof(SpellData);

        private EmptyResults _currentResults = new();
        
        protected override void UpdateCasting() { }

        public override void EvaluateResults() { }

        public override bool TryCast(int casterIndex)
        {
            SpellManager.instance.TryCastSpellServerRpc(casterIndex, _currentResults);
            return true;
        }
    }
}