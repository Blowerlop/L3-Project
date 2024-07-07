using System.Globalization;
using _Project._200_Dev.Utilities.Extensions;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace _Project._200_Dev.LoadingScreen
{
    public class LoadingBar : MonoBehaviour
    {
        [SerializeField] private Image _image;
        [SerializeField] private TMP_Text _text;


        public void SetActive(bool state)
        {
            _image.IsNull()?.gameObject.SetActive(state);
            _text.IsNull()?.gameObject.SetActive(state);
        }
        
        public void UpdateLoadingBar(float loadingValue)
        {
            if (_image != null)
            {
                _image.fillAmount = loadingValue;
            }

            if (_text != null)
            {
                _text.text = $"{loadingValue.ToString(CultureInfo.InvariantCulture)}%";
            }
        }
    }
}
