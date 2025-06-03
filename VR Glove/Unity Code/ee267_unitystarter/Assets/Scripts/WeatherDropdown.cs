using UnityEngine;

public class WeatherDropdown : MonoBehaviour
{
    public GameObject content;

    public void ToggleContent()
    {
        if (content != null)
            content.SetActive(!content.activeSelf);
    }
}
