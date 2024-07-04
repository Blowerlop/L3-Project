using Project._Project.Scripts.StateMachine;
using UnityEngine;

namespace Project._Project.Scripts.Player.States
{
    public class StunState : BaseStateMachineBehaviour
    {
        protected override void OnEnter()
        {
            var navMeshAgent = playerRefs.NavMeshAgent;
            
            navMeshAgent.velocity = Vector3.zero;
            navMeshAgent.isStopped = true;
            navMeshAgent.ResetPath();
            
            playerRefs.NetworkAnimator.Animator.SetBool(Constants.AnimatorsParam.Stunned, true);
        }
        
        protected override void OnExit()
        {
            playerRefs.NetworkAnimator.Animator.SetBool(Constants.AnimatorsParam.Stunned, false);
        }
        
        public override bool CanChangeStateTo<T>()
        {
            return false;
        }

        public override bool CanEnterState(PCPlayerRefs refs)
        {
            return true;
        }

        public override string ToString()
        {
            return "Stun";
        }
    }
}
