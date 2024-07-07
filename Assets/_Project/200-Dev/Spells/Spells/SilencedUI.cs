using _Project._200_Dev.Entities;
using _Project._200_Dev.Entities.Player;
using _Project._200_Dev.User;
using DG.Tweening;
using Unity.Netcode;
using UnityEngine;

namespace _Project._200_Dev.Spells.Spells
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