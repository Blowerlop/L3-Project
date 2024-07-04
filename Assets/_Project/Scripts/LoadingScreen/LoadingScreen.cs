using UnityEngine;
using UnityEngine.UI;

namespace Project
{
    public class LoadingScreen : MonoBehaviour
    {
        [SerializeField] private Image _background;
        [SerializeField] private LoadingBar _loadingBar;


        public void UpdateLoadingScreen(LoadingScreenParameters loadingScreenParameters)
        {
            UpdateBackground(loadingScreenParameters.backgroundSprite, loadingScreenParameters.backgroundColor);
        }
        
        private void UpdateBackground(Sprite sprite = null, Color? color = null)
        {
            _background.sprite = sprite;
            if (color.HasValue) _background.color = color.Value;
        }
    }
}
