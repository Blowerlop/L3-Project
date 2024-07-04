using DG.Tweening;
using Project._Project.Scripts;
using Unity.Netcode;
using UnityEngine;

namespace Project.Spells
{
    public class SilencedUI : MonoBehaviour
    {
        private Entity _entity;
        
        [SerializeField] private CanvasGroup group;
        
        private void Awake()
        {
            if (NetworkManager.Singleton is { IsClient: false }) return;

            UserInstance.Me.OnPlayerLinked += Setup;
        }

        private void OnDestroy()
        {                        
            if (NetworkManager.Singleton is { IsClient: false }) return;
            if (UserInstance.Me != null) UserInstance.Me.OnPlayerLinked -= Setup;
            
            if (_entity != null)
            {
                _entity.OnSilenceChanged -= OnSilenceChanged;
            }
        }

        private void Setup(PlayerRefs refs)
        {
            if (refs is PCPlayerRefs pcRefs)
            {
                _entity = pcRefs.Entity;
                _entity.OnSilenceChanged += OnSilenceChanged;
            }
            
            group.alpha = 0;
        }
        
        private void OnSilenceChanged(bool value)
        {
            group.DOFade(value ? 1 : 0, 0.15f);
        }
    }
}