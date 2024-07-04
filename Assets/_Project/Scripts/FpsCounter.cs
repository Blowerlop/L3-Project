using System.Runtime.CompilerServices;
using Sirenix.OdinInspector;
using TMPro;
using UnityEngine;

namespace Project
{
    public class FpsCounter : MonoBehaviour
    {
        [Title("Configuration")]
        private int _frameCounter;
        [SerializeField] private int _totalFramesCount = 60;
        private int[] _fpsBuffer;
        
        [Title("References")]
        public TMP_Text text;


        private void Start()
        {
            _fpsBuffer = new int[_totalFramesCount];
        }

        private void Update()
        {
            Compute();
        }
        
        
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private void UpdateDisplay(int value)
        {
            text.text = value.ToString();
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private void Compute()
        {
            _frameCounter++;
            
            if (_frameCounter == _totalFramesCount) _frameCounter = 0;

            _fpsBuffer[_frameCounter] = GetCurrentFps();
            UpdateDisplay(Average());
        }
        
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private int GetCurrentFps() => Mathf.RoundToInt(1.0f / Time.deltaTime);

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private int Average()
        {
            int total = 0;
            for (int i = 0; i < _fpsBuffer.Length; i++)
            {
                total += _fpsBuffer[i];
            }

            return total / _fpsBuffer.Length;
        }
    }
}
