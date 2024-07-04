#if UNITY_EDITOR
using UnityEngine;

namespace Project
{
    public sealed class ReadMe : MonoBehaviour
    {
        [SerializeField, TextArea(0, 99)] private string _text;
    }
}
#endif