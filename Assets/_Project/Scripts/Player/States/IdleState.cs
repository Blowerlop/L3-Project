using Project._Project.Scripts.StateMachine;

namespace Project._Project.Scripts.Player.States
{
    public class IdleState : BaseStateMachineBehaviour
    {
        protected override void OnEnter()
        {
            playerRefs.NetworkAnimator.Animator.SetBool(Constants.AnimatorsParam.Movement, false);
        }

        public override bool CanChangeStateTo<T>()
        {
            return true;
        }

        public override bool CanEnterState(PCPlayerRefs refs)
        {
            return true;
        }

        public override string ToString()
        {
            return "Idle";
        }
    }
}
