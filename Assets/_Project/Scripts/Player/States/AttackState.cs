using System;
using Project._Project.Scripts.StateMachine;

namespace Project._Project.Scripts.Player.States
{
    public class AttackState : BaseStateMachineBehaviour
    {
        protected override void OnEnter()
        {
            playerRefs.NetworkAnimator.ResetTrigger(Constants.AnimatorsParam.EndAttackInstant);
            playerRefs.NetworkAnimator.SetTrigger(Constants.AnimatorsParam.Attack);
        }

        protected override void OnExit()
        {
            playerRefs.NetworkAnimator.ResetTrigger(Constants.AnimatorsParam.Attack);
            playerRefs.NetworkAnimator.SetTrigger(Constants.AnimatorsParam.EndAttackInstant);
        }

        public override bool CanChangeStateTo<T>()
        {
            Type type = typeof(T);
            return type == typeof(IdleState) || type == typeof(MoveState) || type == typeof(ChannelingState);
        }

        public override bool CanEnterState(PCPlayerRefs refs)
        {
            // Can't attack if is casting a spell
            var inCastController = refs.InCastController;
            return !inCastController.IsCasting;
        }

        public override string ToString()
        {
            return "Attack";
        }
    }
}