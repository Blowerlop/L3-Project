using System;
using System.Collections;
using System.Linq;
using Sirenix.OdinInspector;
using Unity.Netcode;
using Unity.Netcode.Transports.UTP;
using UnityEngine;

namespace Project
{
    public enum EConnectionState
    {
        EstablishingConnection,
        Connected,
        Connecting,
        Disconnected,
        Disconnecting,
        // TransportFailure
    }

    public class Netcode_ConnectionManager : MonoSingleton<Netcode_ConnectionManager>
    {
        #region Variables
        // [ShowInInspector] private EConnectionState _connectionState = EConnectionState.Disconnected;
        
        private const string DEFAULT_KICK_REASON = "You have been kicked by the server";
        
        [Title("References")]
        private UnityTransport _transport;
        
        #if UNITY_SERVER
        [SerializeField] private bool _startServerAutoIfServerBuild = true;
        #endif
        #endregion

        
        #region Updates
        private IEnumerator Start()
        {
            // GetComponent in start because sometimes the NetworkManager initialize lately
            _transport = NetworkManager.Singleton.GetComponent<UnityTransport>();
            
            NetworkManager.Singleton.OnClientStarted += OnClientStarted;
            NetworkManager.Singleton.OnClientStopped += OnClientStopped;
            NetworkManager.Singleton.OnClientConnectedCallback += OnClientConnectedCallback;
            NetworkManager.Singleton.OnClientDisconnectCallback += OnClientDisconnectCallback;
            NetworkManager.Singleton.OnServerStarted += OnServerStarted;
            NetworkManager.Singleton.OnServerStopped += OnServerStopped;
            NetworkManager.Singleton.OnTransportFailure += OnTransportFailure;
            _transport.OnTransportEvent += OnTransport;

#if UNITY_SERVER
            if (_startServerAutoIfServerBuild)
            {
                yield return null;
                StartServer();
                yield return new WaitForSeconds(5.0f);
                GRPC_NetworkManager.instance.StartClient();
            }
#else
            yield break;
#endif
        }

        protected override void OnDestroy()
        {
            base.OnDestroy();
            
            if (NetworkManager.Singleton == null) return;

            NetworkManager.Singleton.OnClientStarted -= OnClientStarted;
            NetworkManager.Singleton.OnClientStopped -= OnClientStopped;
            NetworkManager.Singleton.OnClientConnectedCallback -= OnClientConnectedCallback;
            NetworkManager.Singleton.OnClientDisconnectCallback -= OnClientDisconnectCallback;
            NetworkManager.Singleton.OnServerStarted -= OnServerStarted;
            NetworkManager.Singleton.OnServerStopped -= OnServerStopped;
            NetworkManager.Singleton.OnTransportFailure -= OnTransportFailure;
            _transport.OnTransportEvent -= OnTransport;
        }
        #endregion


        #region Methods
        #region Callbacks

        private void OnClientStarted()
        {
            Debug.Log($"Trying to establish a connection to {_transport.ConnectionData.Address}:{_transport.ConnectionData.Port}");
            // _connectionState = EConnectionState.EstablishingConnection;
        }

        private void OnClientStopped(bool isHostClient)
        {
            Debug.Log("Connection ended");
            // _connectionState = EConnectionState.Disconnected;
        }

        private void OnClientConnectedCallback(ulong clientId)
        {
            if (NetworkManager.Singleton.IsServer)
            {
                Debug.Log($"Client id {clientId} has connected");
            }
            else
            {
                Debug.Log($"You successfully connected to the server as id {clientId}");
                // _connectionState = EConnectionState.Connected;
            }
        }

        private void OnClientDisconnectCallback(ulong clientId)
        {
            Debug.Log(NetworkManager.Singleton.IsServer
                ? $"Client {clientId} has disconnected"
                : "You successfully disconnected from the server");

            string disconnectReason = NetworkManager.Singleton.DisconnectReason;
            if (string.IsNullOrEmpty(disconnectReason) == false)
            {
                Debug.LogError($"Reason : {disconnectReason}");
            }
        }

        private void OnServerStarted()
        {
            Debug.Log("Server started");
            // _connectionState = EConnectionState.Connected;
        }

        private void OnServerStopped(bool _)
        {
            Debug.Log("Server stopped");
            // _connectionState = EConnectionState.Disconnected;
        }

        private void OnTransportFailure()
        {
            Debug.Log("Transport Failure");
        }

        private void OnTransport(NetworkEvent type, ulong id, ArraySegment<byte> payload, float time)
        {
            if (NetworkManager.Singleton.IsServer)
            {
                switch (type)
                {
                    case NetworkEvent.Data:
                        break;

                    case NetworkEvent.Connect:
                        Debug.Log("A client is connecting...");
                        break;
                
                    case NetworkEvent.Disconnect:
                        Debug.Log("A client is disconnecting...");
                        break;

                    case NetworkEvent.TransportFailure:
                    case NetworkEvent.Nothing:
                    default:
                        Debug.Log(type);
                        break;
                }
            }
            else
            {
                switch (type)
                {
                    case NetworkEvent.Data:
                        break;

                    case NetworkEvent.Connect:
                        Debug.Log("Connection established ! Connecting...");
                        // _connectionState = EConnectionState.Connecting;
                        break;
                
                    case NetworkEvent.Disconnect:
                        Debug.Log("Disconnecting...");
                        // _connectionState = EConnectionState.Disconnecting;
                        break;

                    case NetworkEvent.TransportFailure:
                    case NetworkEvent.Nothing:
                    default:
                        Debug.Log(type);
                        break;
                }
            }
            
        }

        #endregion


        [ConsoleCommand("set_connection_data", "")]
        private static void UpdateConnectionData(string ipAddress, ushort port, string listenAddress = "0.0.0.0")
        {
            NetworkManager.Singleton.GetComponent<UnityTransport>().SetConnectionData(ipAddress, port, listenAddress);
        }


        [ConsoleCommand("start_server", "Start game as a server")]
        public static void StartServer()
        {
            NetworkManager.Singleton.StartServer();
        }

        [ConsoleCommand("start_server_ip", "Start game as a server with a defined ip")]
        public static void StartServer(string ipAddress, ushort port, string listenAddress)
        {
            UpdateConnectionData(ipAddress, port, listenAddress);
            NetworkManager.Singleton.StartServer();
        }

        [ConsoleCommand("start_host", "Start a game as a host")]
        public static void StartHost()
        {
            NetworkManager.Singleton.StartHost();
        }

        [ConsoleCommand("start_host_ip", "Start a game as a host with a defined ip")]
        public static void StartHost(string ipAddress, ushort port, string listenAddress)
        {
            UpdateConnectionData(ipAddress, port, listenAddress);
            NetworkManager.Singleton.StartHost();
        }

        [ConsoleCommand("connect", "Join a game as a client")]
        public static void StartClient()
        {
            NetworkManager.Singleton.StartClient();
        }

        [ConsoleCommand("connect_ip", "Join a game at the defined ip")]
        public static void StartClient(string ipAddress, ushort port)
        {
            UpdateConnectionData(ipAddress, port, null);
            NetworkManager.Singleton.StartClient();
        }

        [ConsoleCommand("disconnect", "Disconnect from the server")]
        public static void Disconnect()
        {
            if (NetworkManager.Singleton.IsListening == false)
            {
                Debug.Log("You are not connected to any server");
                return;
            }

            NetworkManager.Singleton.Shutdown();
        }

        [ConsoleCommand("kick", "Kick a client from the server")]
        public static void Kick(ulong clientId, string reason = default)
        {
            if (NetworkManager.Singleton.IsListening == false)
            {
                Debug.Log("You are not connected to any server");
                return;
            }

            if (NetworkManager.Singleton.IsServer == false)
            {
                Debug.LogWarning("Only the server can kick a client");
                return;
            }
            
            if (NetworkManager.Singleton.ConnectedClientsIds.Contains(clientId) == false)
            {
                Debug.LogWarning($"There is no player with id {clientId}");
                return;
            }

            if (string.IsNullOrEmpty(reason))
            {
                NetworkManager.Singleton.DisconnectClient(clientId, DEFAULT_KICK_REASON);
            }
            else
            {
                NetworkManager.Singleton.DisconnectClient(clientId, reason);
            }
        }
        #endregion
    }
}