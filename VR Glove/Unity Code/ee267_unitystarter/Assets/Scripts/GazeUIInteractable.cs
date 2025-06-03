using UnityEngine;
using UnityEngine.UI;

public class GazeUIInteractable : MonoBehaviour
{
    public GameObject panelToToggle; // assign in Inspector
    private Image image;
    private Color originalColor;

    void Start()
    {
        image = GetComponent<Image>();
        if (image == null)
            image = GetComponentInChildren<Image>();

        if (image != null)
            originalColor = image.color;
    }

    public void OnHover()
    {
        Debug.Log("Hovered: " + gameObject.name);
        if (image != null)
            image.color = new Color(0.4f, 0.4f, 0.4f, 1f); // gray
    }

    public void OnUnhover()
    {
        if (image != null)
            image.color = originalColor;
    }

    public void OnSelect(string gesture)
    {
        Debug.Log($"Gesture '{gesture}' triggered on: {gameObject.name}");

        if (gesture == "select" && panelToToggle != null)
        {
            bool newState = !panelToToggle.activeSelf;
            panelToToggle.SetActive(newState);
            Debug.Log("Toggled panel: " + panelToToggle.name + " to " + newState);
        }
    }
}
