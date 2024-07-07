using Sirenix.OdinInspector.Editor;
using Unity.Netcode;
using UnityEditor;

namespace _Project._200_Dev.Networking
{
    [CustomEditor(typeof(NetworkBehaviour), true)]
    public class OdinNetworkBehaviourEditor : OdinEditor {}
}
