using _Project._200_Dev.Entities.StateMachine;
using _Project._200_Dev.Spells;
using Project;
using UnityEngine;

namespace _Project._200_Dev.Entities.Player.States
{
    public class MoveState : BaseStateMachineBehaviour
    {
        protected override void OnEnter()
        {
            playerRefs.NavMeshAgent.isStopped = false;
        }

        protected override void OnExit()
        {
            var navMeshAgent = playerRefs.NavMeshAgent;
            
            navMeshAgent.velocity = Vector3.zero;
            navMeshAgent.isStopped = true;
            navMeshAgent.ResetPath();
            
            playerRefs.NetworkAnimator.Animator.SetBool(Constants.AnimatorsParam.Movement, false); 
        }

        public override void Update()
        {
            playerRefs.NetworkAnimator.Animator.SetBool(Constants.AnimatorsParam.Movement, true);
        }

        public override bool CanChangeStateTo<T>()
        {
            return true;
        }

        public override bool CanEnterState(PCPlayerRefs refs)
        {
            var inCastController = refs.InCastController;
            return !inCastController.IsCasting || inCastController.CastingFlags.HasFlag(CastingFlags.EnableMovements);
        }

        public override string ToString()
        {
            return "Move";
        }
    }
}
