namespace _Project._200_Dev.Utilities.Extensions
{
    public static class NetcodeExtensions
    {
        public static bool IsClientOnly(this Unity.Netcode.NetworkBehaviour networkBehaviour)
        {
            return networkBehaviour.IsServer == false && networkBehaviour.IsClient;
        }
        
        public static bool IsServerOnly(this Unity.Netcode.NetworkBehaviour networkBehaviour)
        {
            return networkBehaviour.IsServer && networkBehaviour.IsClient == false;
        }
    }
}