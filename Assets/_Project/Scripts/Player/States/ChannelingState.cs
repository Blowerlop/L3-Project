using Project._Project.Scripts.StateMachine;

namespace Project._Project.Scripts.Player.States
{
    public class ChannelingState : BaseStateMachineBehaviour
    {
        private readonly byte _index;
        
        // Used to create an instance with reflection
        public ChannelingState()
        {
            _index = 0;
        }
        
        public ChannelingState(byte index)
        {
            _index = index;
        }
        
        
        protected override void OnEnter()
        {
            playerRefs.NetworkAnimator.Animator.SetBool("Channeling " + _index, true);
        }
        
        protected override void OnExit()
        {
            playerRefs.NetworkAnimator.Animator.SetBool("Channeling " + _index, false);
        }
        
        public override bool CanChangeStateTo<T>()
        {
            return typeof(T) == typeof(IdleState);
        }

        public override bool CanEnterState(PCPlayerRefs refs)
        {
            return true;
        }

        public override string ToString()
        {
            return "Channeling";
        }

        public override bool Equals<T>(T obj)
        {
            return base.Equals(obj) && _index == (obj as ChannelingState)._index;
        }
    }
}
