using System.Collections;
using Project._Project.Scripts.StateMachine;
using Unity.Netcode.Components;
using UnityEngine;

namespace Project._Project.Scripts.Player.States
{
    public class DeadState : BaseStateMachineBehaviour
    {
        private Coroutine _deathCoroutine;
        

        protected override void OnEnter()
        {
            _deathCoroutine = playerRefs.StartCoroutine(DeathCoroutine());
        }

        protected override void OnExit()
        {
            if (_deathCoroutine != null)
            {
                playerRefs.StopCoroutine(_deathCoroutine);
                _deathCoroutine = null;
            }
            
            playerRefs.Entity.Stats.nHealthStat.SetToMaxValue();
            playerRefs.NetworkAnimator.Animator.SetBool(Constants.AnimatorsParam.Dead, false);
            
            
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
            return "Dead";
        }


        private IEnumerator DeathCoroutine()
        {
            playerRefs.NetworkAnimator.Animator.SetBool(Constants.AnimatorsParam.Dead, true);
            
            PlayerManager.instance.OnDeath(playerRefs);
            
            yield return new WaitUntil(() => playerRefs.NetworkAnimator.Animator.GetCurrentAnimatorStateInfo(0).IsName("Death"));
            yield return new WaitUntil(() => playerRefs.NetworkAnimator.Animator.GetCurrentAnimatorStateInfo(0).normalizedTime >= 1);
            
            playerRefs.PlayerTransform.GetComponent<NetworkTransform>().Teleport(new Vector3(999, 999, 999), Quaternion.identity, Vector3.one);
            
            _deathCoroutine = null;
        }
    }
}