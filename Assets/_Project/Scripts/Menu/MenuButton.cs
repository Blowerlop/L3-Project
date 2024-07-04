using TMPro;
using UnityEngine;
using UnityEngine.UI;


namespace Project
{
    public class MenuButton : MonoBehaviour
    {
        public TextMeshProUGUI descriptionText; 
        public Image gradientImage;

        public Color startColor;
        public Color endColor;

        public Color clickStartColor;
        public Color clickEndColor;  
        // Start is called before the first frame update
        void Start()
        {
            // Material in Image Component aren't instance
            gradientImage.material = new Material(gradientImage.material);
            gradientImage.enabled = false;
            if(descriptionText != null) descriptionText.enabled = false;
            gradientImage.material.SetColor("_LeftColor", startColor);
            gradientImage.material.SetColor("_RightColor", endColor);
        }
         
        public void MouseOver()
        {
            if (descriptionText != null) descriptionText.enabled = true;
            gradientImage.enabled = true;
        }

        public void MouseClick()
        {
            gradientImage.material.SetColor("_LeftColor", clickStartColor);
            gradientImage.material.SetColor("_RightColor", clickEndColor);
        }

        public void MouseExit()
        {
            if (descriptionText != null) descriptionText.enabled = false;
            gradientImage.enabled = false ;
            gradientImage.material.SetColor("_LeftColor", startColor);
            gradientImage.material.SetColor("_RightColor", endColor);
        }
    }
}
