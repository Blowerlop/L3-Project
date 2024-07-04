using DG.Tweening;
using Unity.Netcode;
using UnityEngine;

namespace Project.Spells
{
    public class ChannelingUI : MonoBehaviour
    {
        private ChannelingController _channelingController;

        [SerializeField] private CanvasGroup group;
        
        private void Awake()
        {
            if (NetworkManager.Singleton is { IsClient: false }) return;
            
            UserInstance.Me.OnPlayerLinked += Setup;
            
            group.alpha = 0;
        }
        
        private void OnDestroy()
        {
            if (NetworkManager.Singleton is { IsClient: false }) return;
            if (UserInstance.Me != null) UserInstance.Me.OnPlayerLinked -= Setup;
            
            if (!_channelingController) return;
            
            _channelingController.OnServerChannelingStarted -= OnChannelingStarted;
            _channelingController.OnServerChannelingEnded -= OnChannelingEnded;
        }
        
        private void Setup(PlayerRefs refs)
        {
            _channelingController = refs.Channeling;
            
            _channelingController.OnServerChannelingStarted += OnChannelingStarted;
            _channelingController.OnServerChannelingEnded += OnChannelingEnded;
        }
        
        private void OnChannelingStarted()
        {
            group.DOFade(1, 0.15f);
        }
        
        private void OnChannelingEnded()
        {
            group.DOFade(0, 0.15f);
        }
    }
}