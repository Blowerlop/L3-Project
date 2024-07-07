using System.Collections.Generic;
using _Project._200_Dev.Entities.Player.Stats;

namespace _Project._200_Dev.Entities.Player.Interfaces
{
    public interface IEffectable
    {
        public Entity AffectedEntity { get; }
        
        public IList<Effect> AppliedEffects { get; }
        
        public void SrvAddEffect(Effect effect);
        public void SrvRemoveEffect(Effect effect);
        
        public void SrvCleanse();
        public void SrvDebuff();
    }
}