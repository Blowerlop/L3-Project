using Sirenix.OdinInspector;
using Unity.Netcode;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Project
{
    public class EmotesWheelUI : MonoBehaviour
    {
        private PCPlayerRefs playerRefs;
        
        [SerializeField] private GameObject content;

        [RequiredListLength(4)]
        [SerializeField] private EmotesWheelItem[] items;
        
        private bool _isOpen;
        
        private EmotesWheelItem _selectedItem;

        private void Awake()
        {
            if (NetworkManager.Singleton is { IsClient: false }) return;
            
            UserInstance.Me.OnPlayerLinked += Setup;
        }

        private void Start()
        {
            if (NetworkManager.Singleton is { IsClient: false }) return;
            
            content.SetActive(false);
            
            InputManager.instance.onEmotesWheel.started += OpenEmotesWheel;
            InputManager.instance.onEmotesWheel.canceled += CloseEmotesWheel;
        }

        private void Setup(PlayerRefs refs)
        {
            if(refs is not PCPlayerRefs pcRefs) return;

            playerRefs = pcRefs;
            
            int index = 0;
            
            foreach (var item in items)
            {
                item.Init(this, pcRefs.EmoteController.GetEmote(index), index);
                index++;
            }
        }

        private void OnDestroy()
        {
            if (NetworkManager.Singleton is { IsClient: false }) return;
            if (!InputManager.IsInstanceAlive()) return;
            
            InputManager.instance.onEmotesWheel.started -= OpenEmotesWheel;
            InputManager.instance.onEmotesWheel.canceled -= CloseEmotesWheel;
        }

        private void OpenEmotesWheel(InputAction.CallbackContext obj)
        {
            if (_isOpen) return;

            _selectedItem = null;
            
            content.SetActive(true);
            _isOpen = true;
        }
        
        private void CloseEmotesWheel(InputAction.CallbackContext obj)
        {
            if (!_isOpen) return;

            if (_selectedItem != null)
            {
                if (NetworkManager.Singleton.IsClient)
                {
                    playerRefs.EmoteController.TryPlayEmoteServerRpc(_selectedItem.Index);
                }
                else if (NetworkManager.Singleton.IsServer)
                {
                    playerRefs.EmoteController.TryPlayEmote(_selectedItem.Index);
                }
            }
            
            content.SetActive(false);
            _isOpen = false;
        }

        public void SetItemSelected(EmotesWheelItem item)
        {
            _selectedItem = item;
        }
    }
}