using System;
using _Project._200_Dev.Entities.Player;

namespace _Project._200_Dev.Entities.StateMachine
{
    [Serializable]
    public abstract class BaseStateMachineBehaviour : IDisposable
    {
        protected PCPlayerRefs playerRefs { get; private set; }
        

        public void Enter(PCPlayerRefs refs)
        {
            playerRefs = refs;
            
            OnEnter();
        }
        
        protected virtual void OnEnter() {}

        public virtual void Update() {}

        public void Exit()
        {
            OnExit();
            Dispose();
        }
        
        protected virtual void OnExit() {} 
        
        /// <summary>
        /// 
        /// </summary>
        /// <returns>
        /// <para><b>-true</b>: if the change is allowed</para>
        /// <para><b>-false</b>: if the change is not allowed</para>
        /// </returns>
        public abstract bool CanChangeStateTo<T>() where T : BaseStateMachineBehaviour;
        
        public abstract bool CanEnterState(PCPlayerRefs refs);
        
        public void Dispose()
        {
            playerRefs = null;
            OnDispose();
        }
        
        public virtual void OnDispose() 
        {   
        }
        
        public abstract override string ToString();
        
        public virtual bool Equals<T>(T obj) where T : BaseStateMachineBehaviour
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            
            return GetType() == obj.GetType();
        }
    }
}
