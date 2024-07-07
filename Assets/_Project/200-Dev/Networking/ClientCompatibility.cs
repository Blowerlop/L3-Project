#if UNITY_EDITOR
using _Project._200_Dev.Utilities.Git;
using Sirenix.OdinInspector;
using Unity.Netcode;

namespace _Project._200_Dev.Networking
{
    public class ClientCompatibility : NetworkBehaviour
    {
        [ShowInInspector, ReadOnly] private string _gitCommitHash;
        
        
        public override void OnNetworkSpawn()
        {
            _gitCommitHash = GetGitCommitHash();
            
            if (IsClient == false) return;
            
            CompareCommitHashWithServer_ServerRpc(NetworkManager.Singleton.LocalClientId, _gitCommitHash);
        }

        
        [ServerRpc(RequireOwnership = false)]
        private void CompareCommitHashWithServer_ServerRpc(ulong clientId, string clientGitCommitHash)
        {
            if (_gitCommitHash != clientGitCommitHash)
            {
                NetworkManager.Singleton.DisconnectClient(clientId, $"Last git commit hash different from server : [Server] {_gitCommitHash} / [Client] {clientGitCommitHash}");
                UnityEngine.Debug.LogError($"Client {clientId} disconnected because of different git commit hash");
            }
        }

        private string GetGitCommitHash()
        {
            string commitHash = GitUtilities.RetrieveCurrentCommitShorthash();
            return commitHash;
        }
    }
} 
#endif