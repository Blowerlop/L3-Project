using System.Collections.Generic;
using System.Linq;
using _Project._200_Dev.Entities;
using _Project._200_Dev.Networking.IL_Weaving.Server;
using _Project._200_Dev.User;
using _Project._200_Dev.Utilities.Extensions;
using TMPro;
using Unity.Netcode;
using UnityEngine;
using UnityEngine.UI;

namespace _Project._200_Dev.Lobby
{
    public class TeamsDisplay : NetworkBehaviour
    {
        private readonly Dictionary<int, Image> _playersAvatar = new Dictionary<int, Image>();
        [SerializeField] private Sprite _defaultSprite;
        [SerializeField] private GameObject _template;
        [SerializeField] private bool _ignoreLocalPlayer;


        private void Awake()
        {
            Lobby.instance.OnStateChange += OnLobbyStateChange_UpdateUI;
        }

        public override void OnDestroy()
        {
            base.OnDestroy();
            
            if (Lobby.IsInstanceAlive()) Lobby.instance.OnStateChange -= OnLobbyStateChange_UpdateUI;
        }
        
        private void CliInit()
        {
            SpawnUi();
            SetPlayersAvatar();
        }

        private void SpawnUi()
        {
            for (int i = 0; i < TeamManager.MAX_TEAM; i++)
            {
                Image instance = Instantiate(_template, transform).GetComponentInChildren<Image>();
                instance.transform.parent.GetComponentInChildren<TMP_Text>().text = TeamManager.instance.GetTeamData(i).TryGetUserInstance(out UserInstance userInstance) ? userInstance.PlayerName : "UnknowName";
                _playersAvatar.Add(i, instance);
            }
            
            if (_ignoreLocalPlayer) _playersAvatar[UserInstance.Me.Team].transform.parent.gameObject.SetActive(false);
        }

        private void SetPlayersAvatar()
        {
            var teams = TeamManager.instance.GetTeamsData();
            
            for (int i = 0; i < TeamManager.MAX_TEAM; i++)
            {
                if (TeamManager.instance.IsTeamPlayerSlotAvailable(i))
                {
                    GetAvatarByTeamId(i).sprite = _defaultSprite;
                }
                else
                {
                    int playerCharacterId = UserInstanceManager.instance.GetUserInstance(teams[i].pcPlayerOwnerClientId).CharacterId;

                    GetAvatarByTeamId(i).sprite = SOCharacter.TryGetCharacter(playerCharacterId, out SOCharacter characterData) ? characterData.avatar : _defaultSprite;
                }
            }
        }

        private Image GetAvatarByTeamId(int teamId) => _playersAvatar[teamId];

        private Image GetByPlayerId(int playerId)
        {
            var teams = TeamManager.instance.GetTeamsData();
            return GetAvatarByTeamId(teams.FindIndex(x => x.pcPlayerOwnerClientId == playerId));
        }

        [Server]
        public void SrvAvatar(int playerId, int characterId)
        {
            SetAvatarClientRpc(playerId, characterId);
        }

        [ClientRpc]
        private void SetAvatarClientRpc(int playerId, int characterId)
        {
            SetAvatarLocal(playerId, characterId);
        }

        private void SetAvatarLocal(int playerId, int characterId)
        {
            if (_playersAvatar == null || _playersAvatar.Any() == false) return;
            
            Image player = GetByPlayerId(playerId);

            if (SOCharacter.TryGetCharacter(characterId, out SOCharacter characterData))
            {
                player.sprite = characterData.avatar;
            }
            else Debug.LogError("No character with id " + characterId);
        }
        
        private void OnLobbyStateChange_UpdateUI(ELobbyState lobbyState)
        {
            if (lobbyState == ELobbyState.CharacterSelection)
            {
                if (IsClient) CliInit();

                Lobby.instance.OnStateChange -= OnLobbyStateChange_UpdateUI;
            }
            
        }
    }
}
