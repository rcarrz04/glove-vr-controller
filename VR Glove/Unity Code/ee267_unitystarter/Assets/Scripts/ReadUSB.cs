using System;
using System.IO;
using System.IO.Ports;
using UnityEngine;

public class ReadUSB : MonoBehaviour
{
    const int baudrate = 115200;
    const string portName = "/dev/cu.usbmodem170938401";

    // Declare game objects
    private SerialPort serialPort = new SerialPort(portName, baudrate);
    private GameObject weatherPanel;
    private GameObject weatherContent;
    public GameObject galleryPanel;
    public GalleryManager galleryController;
    private GameObject targetGameApp;


    private float lastGestureTime = 0f;
    public float gestureDebounceDuration = 0.3f;

    private bool swipeInProgress = false;

    void Start()
    {
        serialPort.ReadTimeout = 100;

        try
        {
            serialPort.Open();
            Debug.Log("Serial port opened: " + portName);
        }
        catch (Exception e)
        {
            Debug.LogError("Failed to open serial port: " + e.Message);
        }

        weatherPanel = GameObject.Find("WeatherPanel");
        weatherContent = GameObject.Find("WeatherContent");
        galleryPanel = GameObject.Find("GalleryPanel");
        targetGameApp = GameObject.Find("TargetGameApp");

        // Initialize Game Objects
        if (weatherPanel != null) weatherPanel.SetActive(true);
        if (weatherContent != null) weatherContent.SetActive(false);
        if (galleryPanel != null)
        {
            galleryPanel.SetActive(false);
            galleryController = galleryPanel.GetComponentInChildren<GalleryManager>();

            if (galleryController != null)
                Debug.Log("GalleryManager successfully assigned.");
            else
                Debug.LogWarning("GalleryManager NOT found under GalleryPanel!");
        }
        else
        {
            Debug.LogWarning("GalleryPanel not found!");
        }

    }

    void Update()
    {
        try
        {
            // Parse quaternion and gesture data from serial
            if (!serialPort.IsOpen) return;

            string incoming = serialPort.ReadExisting();
            string[] lines = incoming.Split('\n');

            foreach (string raw in lines)
            {
                string line = raw.Trim();
                if (string.IsNullOrEmpty(line)) continue;

                Debug.Log("Serial line: " + line);

                if (line.StartsWith("QC"))
                {
                    string[] parts = line.Split(' ');

                    if (parts.Length >= 5 &&
                        float.TryParse(parts[1], out float w) &&
                        float.TryParse(parts[2], out float x) &&
                        float.TryParse(parts[3], out float y) &&
                        float.TryParse(parts[4], out float z))
                    {
                        Quaternion q = new Quaternion(-x, y, z, w);
                        Vector3 angles = Quaternion.Inverse(q).eulerAngles;
                        transform.rotation = Quaternion.Euler(angles.x, angles.y, -angles.z);
                    }

                    int gestureIndex = Array.IndexOf(parts, "GESTURE");
                    if (gestureIndex != -1 && gestureIndex + 1 < parts.Length)
                    {
                        string gesture = parts[gestureIndex + 1].ToLower();
                        HandleGesture(gesture);
                    }
                }
            }
        }
        catch (TimeoutException) { }
        catch (IOException ioEx)
        {
            Debug.LogWarning("Serial IO Error: " + ioEx.Message);
        }
        catch (Exception ex)
        {
            Debug.LogWarning("Serial read failed: " + ex.Message);
        }
    }

    // Process gesture data
    void OnApplicationQuit()
    {
        if (serialPort != null && serialPort.IsOpen)
        {
            serialPort.Close();
            Debug.Log("Serial port closed.");
        }
    }

    private void HandleGesture(string gesture)
    {
        if (gesture == "swiper")
        {
            if (swipeInProgress) return;
            swipeInProgress = true;

            Debug.Log("Inside SwipeR block");
            if (galleryController != null)
            {
                Debug.Log("Calling ShowNext()");
                galleryController.ShowNext();
            }
            else
            {
                Debug.LogWarning("galleryController is null");
            }

            Invoke(nameof(ResetSwipeLock), 0.6f);
            return;
        }

        if ((Time.time - lastGestureTime < gestureDebounceDuration))
            return;

        lastGestureTime = Time.time;
        Debug.Log("Gesture detected: " + gesture);

        if (GazeManager.currentTarget != null)
        {
            var interactable = GazeManager.currentTarget.GetComponent<GazeUIInteractable>();
            if (interactable != null)
                interactable.OnSelect(gesture);

            // Toggle weather dropdown
            if (GazeManager.currentTarget.name == "ToggleButton" && gesture == "select")
            {
                if (weatherContent != null)
                    weatherContent.SetActive(!weatherContent.activeSelf);
            }

            // Toggle gallery panel
            if (GazeManager.currentTarget.name == "PhotosApp" && gesture == "select")
            {
                if (galleryPanel != null)
                    galleryPanel.SetActive(!galleryPanel.activeSelf);

                if (weatherContent != null)
                    weatherContent.SetActive(false);

                if (weatherPanel != null)
                    weatherPanel.SetActive(!weatherPanel.activeSelf);

                if (targetGameApp != null)
                    targetGameApp.SetActive(!targetGameApp.activeSelf);
            }

            // Start target game
            if (GazeManager.currentTarget.name == "TargetGameApp" && gesture == "select")
            {
                var toggle = GazeManager.currentTarget.GetComponent<SpawnerToggle>();

                if (toggle != null)
                    toggle.StartGame();
            }


            // Destroy target if gazing at a spawned ball and "select" is triggered
            if (gesture == "select" && GazeManager.currentTarget != null)
            {
                var target = GazeManager.currentTarget.GetComponent<TargetBehavior>();
                if (target != null)
                {
                    Debug.Log("Destroying: " + GazeManager.currentTarget.name);
                    target.DestroyTarget();
                }
            }

        }
    }

    private void ResetSwipeLock()
    {
        swipeInProgress = false;
    }
}
