using _Project._200_Dev.Entities.StateMachine;
using Project;

namespace _Project._200_Dev.Entities.Player.States
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
