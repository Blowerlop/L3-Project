using System.Collections.Generic;
using Sirenix.OdinInspector;
using Unity.Netcode;
using UnityEngine;

namespace Project
{
    public class RTT : NetworkBehaviour
    {
        [SerializeField] private float _pingInterval = 0.5f;
        [ShowInInspector, ReadOnly] private float _currentRTT;
        private ulong _clientId;
        private float _lastPingTime;
        
        
        public override void OnNetworkSpawn()
        {
            if (IsServer || IsOwner == false)
            {
                enabled = false;
                return;
            }

            _clientId = NetworkManager.Singleton.LocalClientId;
        }
        
        
        private void FixedUpdate()
        {
            if (Time.realtimeSinceStartup - _lastPingTime > _pingInterval)
            {
                _lastPingTime = Time.realtimeSinceStartup;
                PingServerRpc(_lastPingTime, _clientId);
            }
            
        }
        

        [ServerRpc(RequireOwnership = false)]
        private void PingServerRpc(float pingTime, ulong senderClientId)
        {
            ClientRpcParams targetClient = new ClientRpcParams
            {
                Send = new ClientRpcSendParams { TargetClientIds = new List<ulong> { senderClientId } }
            };
            
            PongClientRpc(pingTime, targetClient);
        }

        [ClientRpc]
        private void PongClientRpc(float pingTime, ClientRpcParams clientRpcParams = default)
        {
            _currentRTT = Mathf.Round((Time.realtimeSinceStartup - pingTime) * 1000);
        }
    }
}