using System.Collections.Generic;
using Project._Project.Scripts;

namespace Project
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