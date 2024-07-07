using Unity.Netcode;

namespace _Project._200_Dev.Networking.IL_Weaving.Server
{
    public static class ServerMethods
    {
        public static bool IsNetworkManagerSingletonExist()
        {
            return NetworkManager.Singleton != null;
        }

        public static bool IsListening()
        {
            return NetworkManager.Singleton.IsListening;
        }

        public static bool IsServer()
        {
            return NetworkManager.Singleton.IsServer; 
        }
    }
}