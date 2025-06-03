using UnityEngine;
using UnityEngine.UI;

public class GalleryManager : MonoBehaviour
{
    public Sprite[] galleryImages;         // Assign in Inspector
    public Image displayImage;             // Assign in Inspector
    private int currentIndex = 0;

    public void ShowNext()
    {
        if (galleryImages.Length == 0) return;
        currentIndex = (currentIndex + 1) % galleryImages.Length;
        displayImage.sprite = galleryImages[currentIndex];
    }

    public void ShowPrevious()
    {
        if (galleryImages.Length == 0) return;
        currentIndex = (currentIndex - 1 + galleryImages.Length) % galleryImages.Length;
        displayImage.sprite = galleryImages[currentIndex];
    }

    void Start()
    {
        if (galleryImages.Length > 0 && displayImage != null)
            displayImage.sprite = galleryImages[currentIndex];
    }
}
