using Unity.Netcode;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Project
{
    /// <summary>
    /// SINGLE EMOTE SCRIPT, REPRENDRE L'EMOTE WHEEL POUR PLUS TARD
    /// </summary>
    public class SingleEmoteTrigger : MonoBehaviour
    {
        private PCPlayerRefs playerRefs;

        private void Awake()
        {
            if (NetworkManager.Singleton is { IsClient: false }) return;

            UserInstance.Me.OnPlayerLinked += Setup;
        }

        private void Start()
        {
            if (NetworkManager.Singleton is { IsClient: false }) return;
            InputManager.instance.onEmotesWheel.canceled += CloseEmotesWheel;
        }

        private void Setup(PlayerRefs refs)
        {
            if (refs is not PCPlayerRefs pcRefs) return;

            playerRefs = pcRefs;
        }

        private void OnDestroy()
        {
            if (NetworkManager.Singleton is { IsClient: false }) return;
            if (!InputManager.IsInstanceAlive()) return;

            InputManager.instance.onEmotesWheel.canceled -= CloseEmotesWheel;
        }

        private void CloseEmotesWheel(InputAction.CallbackContext obj)
        {
            if (NetworkManager.Singleton.IsClient)
            {
                playerRefs.EmoteController.TryPlayEmoteServerRpc(0);
            }
            else if (NetworkManager.Singleton.IsServer)
            {
                playerRefs.EmoteController.TryPlayEmote(0);
            }
        }
    }
}