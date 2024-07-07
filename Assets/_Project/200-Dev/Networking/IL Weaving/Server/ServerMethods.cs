using Unity.Netcode;

namespace Project
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