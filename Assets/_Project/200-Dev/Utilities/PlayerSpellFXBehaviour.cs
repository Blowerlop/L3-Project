using UnityEngine;

namespace Project
{
    public class PlayerSpellFXBehaviour : StateMachineBehaviour
    {
        private PlayerSpellsAnimFXHandler _fxHandler;

        public override void OnStateEnter(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
        {
            base.OnStateEnter(animator, stateInfo, layerIndex);
            
            _fxHandler ??= animator.GetComponentInParent<PlayerSpellsAnimFXHandler>();
            
            _fxHandler?.OnStateEnter(stateInfo.shortNameHash);
        }
        
        public override void OnStateExit(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
        {
            base.OnStateExit(animator, stateInfo, layerIndex);
            
            _fxHandler?.OnStateExit(stateInfo.shortNameHash);
        }
    }
}
