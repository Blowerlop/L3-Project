#if UNITY_EDITOR
using UnityEngine;

namespace _Project._200_Dev.Utilities
{
    public sealed class ReadMe : MonoBehaviour
    {
        [SerializeField, TextArea(0, 99)] private string _text;
    }
}
#endif