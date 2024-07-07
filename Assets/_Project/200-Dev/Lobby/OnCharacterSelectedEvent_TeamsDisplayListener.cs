using UnityEngine;

namespace _Project._200_Dev.Lobby
{
    public class OnCharacterSelectedEvent_TeamsDisplayListener : MonoBehaviour
    {
        private TeamsDisplay _teamsDisplay;


        private void Awake()
        {
            _teamsDisplay = FindAnyObjectByType<TeamsDisplay>();
        }

        private void Start()
        {
            CharacterSelectionUI.SrvOnCharacterSelectedEvent += OnCharacterSelected_SetPlayerAvatar;
        }

        private void OnDestroy()
        {
            CharacterSelectionUI.SrvOnCharacterSelectedEvent -= OnCharacterSelected_SetPlayerAvatar;
        }

        private void OnCharacterSelected_SetPlayerAvatar(int playerId, int characterId)
        {
            _teamsDisplay.SrvAvatar(playerId, characterId);
        }
    }
}
