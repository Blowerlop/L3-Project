using Sirenix.OdinInspector.Editor;
using Unity.Netcode;
using UnityEditor;

namespace Project
{
    [CustomEditor(typeof(NetworkBehaviour), true)]
    public class OdinNetworkBehaviourEditor : OdinEditor {}
}
