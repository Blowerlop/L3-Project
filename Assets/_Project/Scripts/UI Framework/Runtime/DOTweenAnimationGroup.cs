using DG.Tweening;
using Sirenix.OdinInspector;
using Sirenix.Utilities;
using UnityEngine;

namespace Project.Scripts.UIFramework
{
    public class DOTweenAnimationGroup : MonoBehaviour
    {
        [SerializeField, ReadOnly] private DOTweenAnimation[] _doTweenAnimations;
        private bool _hasPlayedForward;

        private void OnValidate()
        {
            Fetch();
        }


        [Button]
        private void Fetch()
        {
            _doTweenAnimations = GetComponentsInChildren<DOTweenAnimation>();
        }
        
        public void RewindThenRecreateTween()
        {
            _doTweenAnimations.ForEach(x => x.RewindThenRecreateTween());
        }
        
        public void RewindThenRecreateTweenAndPlay()
        {
            _doTweenAnimations.ForEach(x => x.RewindThenRecreateTweenAndPlay());
        }

        public void RecreateTween()
        {
            _doTweenAnimations.ForEach(x => x.RecreateTween());
        }

        public void RecreateTweenAndPlay()
        {
            _doTweenAnimations.ForEach(x => x.RecreateTweenAndPlay());
        }

        public void DOPlay()
        {
            _doTweenAnimations.ForEach(x => x.DOPlay());
        }

        public void DOPlayBackwards()
        {
            _hasPlayedForward = false;
            _doTweenAnimations.ForEach(x => x.DOPlayBackwards());
        }

        public void DOPlayForward()
        {
            _hasPlayedForward = true;
            _doTweenAnimations.ForEach(x => x.DOPlayForward());
        }

        public void DOTogglePlayDirection()
        {
            if (_hasPlayedForward) DOPlayBackwards();
            else DOPlayForward();
        }

        public void DOPause()
        {
            _doTweenAnimations.ForEach(x => x.DOPause());
        }

        public void DOTogglePause()
        {
            _doTweenAnimations.ForEach(x => x.DOTogglePause());
        }

        public void DORewind()
        {
            _doTweenAnimations.ForEach(x => x.DORewind());
        }

        public void DORestart()
        {
            _doTweenAnimations.ForEach(x => x.DORestart());
        }

        public void DORestart(bool fromHere)
        {
            _doTweenAnimations.ForEach(x => x.DORestart(fromHere));
        }

        public void DOComplete()
        {
            _doTweenAnimations.ForEach(x => x.DOComplete());
        }

        public void DOKill()
        {
            _doTweenAnimations.ForEach(x => x.DOKill());
        }

        public void DOPlayById(string id)
        {
            _doTweenAnimations.ForEach(x => x.DOPlayById(id));
        }

        public void DOPlayAllById(string id)
        {
            _doTweenAnimations.ForEach(x => x.DOPlayAllById(id));
        }

        public void DOPauseAllById(string id)
        {
            _doTweenAnimations.ForEach(x => x.DOPauseAllById(id));
        }

        public void DOPlayBackwardsById(string id)
        {
            _doTweenAnimations.ForEach(x => x.DOPlayBackwardsById(id));
        }
        
        public void DOPlayBackwardsAllById(string id)
        {
            _doTweenAnimations.ForEach(x => x.DOPlayBackwardsAllById(id));
        }

        public void DOPlayForwardById(string id)
        {
            _doTweenAnimations.ForEach(x => x.DOPlayForwardById(id));
        }

        public void DOPlayForwardAllById(string id)
        {
            _doTweenAnimations.ForEach(x => x.DOPlayForwardAllById(id));
        }

        public void DOPlayNext()
        {
            _doTweenAnimations.ForEach(x => x.DOPlayNext());
        }

        public void DORewindAndPlayNext()
        {
            _doTweenAnimations.ForEach(x => x.DORewindAndPlayNext());

        }

        public void DORewindAllById(string id)
        {
            _doTweenAnimations.ForEach(x => x.DORewindAllById(id));

        }

        public void DORestartById(string id)
        {
            _doTweenAnimations.ForEach(x => x.DORestartById(id));
        }

        public void DORestartAllById(string id)
        {
            _doTweenAnimations.ForEach(x => x.DORestartAllById(id));
        }

        public void DOKillById(string id)
        {
            _doTweenAnimations.ForEach(x => x.DOKillById(id));
        }

        public void DOKillAllById(string id)
        {
            _doTweenAnimations.ForEach(x => x.DOKillAllById(id));
        }
    }
}