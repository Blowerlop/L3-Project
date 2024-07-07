using System.Linq;
using _Project._200_Dev.Entities.Player;
using _Project._200_Dev.Entities.Player.States;
using _Project._200_Dev.Entities.Projectile;
using _Project._200_Dev.LoadingScreen;
using _Project._200_Dev.Logs.Game_DataLogger;
using _Project._200_Dev.Managers;
using _Project._200_Dev.Spells;
using _Project._200_Dev.UI;
using _Project._200_Dev.User;
using _Project._200_Dev.Utilities;
using _Project._200_Dev.Utilities.NetworkBehaviour;
using DG.Tweening;
using Unity.Netcode;
using UnityEngine;
using UnityEngine.SceneManagement;
using SceneManager = _Project._200_Dev.Managers.SceneManager;

namespace _Project._200_Dev.Game
{
    public class Gameloop : NetworkSingleton<Gameloop>
    {
        private readonly NetworkVariable<bool> _isGameRunning = new NetworkVariable<bool>(false);

        public static bool IsGameRunning => instance._isGameRunning.Value;

        [SerializeField] private float roundEndTime = 2f;
        [SerializeField] private float roundStartTime = 3f;

        public override void OnNetworkSpawn()
        {
            if (!IsServer) return;

            PlayerManager.OnPlayerDied += OnPlayerDied;
            PlayerManager.OnAllPlayersReady += StartNewRound;
        }

        public override void OnNetworkDespawn()
        {
            PlayerManager.OnPlayerDied -= OnPlayerDied;
            PlayerManager.OnAllPlayersReady -= StartNewRound;
        }

        private void OnPlayerDied(PlayerRefs refs)
        {
            var players = PlayerManager.instance.players;

            var alivePlayers = players.FindAll(p => p is PCPlayerRefs pcPlayerRefs && pcPlayerRefs.StateMachine.currentState is not DeadState);

            if (alivePlayers.Count != 1) return;

            OnLastPlayerAlive(alivePlayers[0]);
        }

        private void OnLastPlayerAlive(PlayerRefs refs)
        {
            bool endGame = false;

            var pcUser = UserInstanceManager.instance.GetUsersInstance().FirstOrDefault(u => u.Team == refs.TeamIndex);

            if (!pcUser)
            {
                Debug.LogError("No PC user found for team " + refs.TeamIndex);
                return;
            }

            pcUser.WinCount.Value++;

            endGame = pcUser.WinCount.Value >= 2;
            

            string winnerPlayers = pcUser == null ? "" : pcUser.PlayerName;
            ShowWinText(winnerPlayers, endGame);

            EndCurrentRound(endGame);
            OnRoundEndedClientRpc(winnerPlayers, endGame, pcUser.Team);
        }

        [ClientRpc]
        private void OnRoundEndedClientRpc(string winnerNames, bool gameFinished = false, int TeamIndex = -1)
        {
            if (IsHost) return;

            ShowWinText(winnerNames, gameFinished);

            if (gameFinished)
            {
                DataLogger.LogTeamInfo(TeamIndex);
                DOVirtual.DelayedCall(roundEndTime, () =>
                {
                    NetworkManager.Shutdown();
                    UnityEngine.SceneManagement.SceneManager.LoadSceneAsync(0);
                });
            }
        }

        private void ShowWinText(string winnerNames, bool gameFinished = false)
        {
            PlaceholderLabel.instance.SetText($"Team {winnerNames} win " + (gameFinished ? "this game ! " : "this round !"), 1.9f);
        }

        private void EndCurrentRound(bool endGame)
        {
            _isGameRunning.Value = false;
            
            SoundManager.instance.PlayGlobalSound("EndRound", "end", SoundManager.EventType.UI);

            DOVirtual.DelayedCall(roundEndTime, () =>
            {
                if (endGame)
                {
                    // TODO: Need to reset some managers
                    SceneManager.Network_LoadSceneAsync("Lobby", LoadSceneMode.Single, new LoadingScreenParameters(null, Color.black));
                    return;
                }
                
                ResetRound();
                StartNewRound();
            });
        }

        private void ResetRound()
        {
            var spellManager = SpellManager.instance;
            
            spellManager.SrvResetSpells();
            spellManager.SrvResetCasts();
            
            Projectile.SrvResetProjectiles();
            
            var playerManager = PlayerManager.instance;
            
            playerManager.ResetPlayers();
            playerManager.PlacePlayers();
        }
        
        private void StartNewRound()
        {
            Timer timer = new Timer();
            
            OnRoundStartClientRpc();
            
            timer.StartTimerWithUpdateCallback(this, roundStartTime + 1f, (value) =>
            {
                PlaceholderLabel.instance.SetText($"Round starting in {value}");
            }, () =>
            {
                _isGameRunning.Value = true;
                PlaceholderLabel.instance.SetText("Fight !", 1.5f);
            }, ceiled: true);
        }
        
        // Maybe not that great but flemme to netvar
        [ClientRpc]
        private void OnRoundStartClientRpc()
        {
            SoundManager.instance.PlayGlobalSound("CountDown", "timer", SoundManager.EventType.UI);
            
            if (IsHost) return;
            
            Timer timer = new Timer();
            
            timer.StartTimerWithUpdateCallback(this, 4f, (value) =>
            {
                PlaceholderLabel.instance.SetText($"Round starting in {value}");
            }, () =>
            {
                PlaceholderLabel.instance.SetText("Fight !", 1.5f);
            }, ceiled: true);
        }
    }
}
