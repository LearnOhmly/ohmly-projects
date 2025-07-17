# ------------------ IMPORTS ------------------
import serial         # For communicating with Arduino
import time           # For sleep delays and timing
import cv2            # For camera input and image processing
import mediapipe as mp  # For hand tracking
import keyboard       # For manual WASD key input
import random         # For random movement when no hand is detected

# ------------------ LOAD PAN/TILT OFFSETS ------------------
try:
    with open("offsets.txt", "r") as f:
        PAN_OFFSET = int(f.readline())   # Custom horizontal offset
        TILT_OFFSET = int(f.readline())  # Custom vertical offset
except:
    PAN_OFFSET = 0
    TILT_OFFSET = 0

# ------------------ SERIAL SETUP ------------------
arduino = serial.Serial('COM3', 9600)  # Replace with your COM port
time.sleep(2)  # Give Arduino time to reset

# ------------------ MEDIAPIPE SETUP ------------------
cap = cv2.VideoCapture(0)  # Use default webcam
mp_hands = mp.solutions.hands
hands = mp_hands.Hands()  # Initialize hand tracking
mp_drawing = mp.solutions.drawing_utils  # Optional visual overlay

print("Press W/S/A/D to adjust offset. Press ESC to quit.")

# ------------------ MAIN LOOP ------------------
while True:
    ret, frame = cap.read()
    if not ret:
        continue  # Skip if webcam fails

    frame = cv2.flip(frame, 1)  # Mirror the image for natural movement
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)  # Convert to RGB for MediaPipe
    results = hands.process(rgb)  # Detect hands in the frame

    # ------------- WASD OFFSET CONTROLS (manually fine-tune servo aim) -------------
    if keyboard.is_pressed('w'):
        TILT_OFFSET -= 1
        print(f"TILT_OFFSET: {TILT_OFFSET}")
        time.sleep(0.1)
    if keyboard.is_pressed('s'):
        TILT_OFFSET += 1
        print(f"TILT_OFFSET: {TILT_OFFSET}")
        time.sleep(0.1)
    if keyboard.is_pressed('a'):
        PAN_OFFSET -= 1
        print(f"PAN_OFFSET: {PAN_OFFSET}")
        time.sleep(0.1)
    if keyboard.is_pressed('d'):
        PAN_OFFSET += 1
        print(f"PAN_OFFSET: {PAN_OFFSET}")
        time.sleep(0.1)

    # ------------- OBJECT DETECTION AND TRACKING -------------
    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            # Draw hand landmarks for debugging
            mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)

            # Get position of index finger tip (normalized between 0.0 and 1.0)
            x = hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP].x
            y = hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP].y

            # Convert x/y to angles and apply offsets
            pan_angle = int(x * 180) + PAN_OFFSET
            tilt_angle = int(y * 180) + TILT_OFFSET

            # Clamp to valid servo range (0 to 180 degrees)
            pan_angle = max(0, min(180, pan_angle))
            tilt_angle = max(0, min(180, tilt_angle))

            # Send angles and detection signal to Arduino
            arduino.write(f"{pan_angle},{tilt_angle}\n".encode())
            arduino.write(b"DETECTED\n")
            print(f"Sent: {pan_angle},{tilt_angle}")

    else:
        # ------------- RANDOM SCANNING MODE WHEN NO HAND IS DETECTED -------------
        pan_angle = random.randint(30, 180)
        tilt_angle = random.randint(130, 180)

        # Send random scan movement to Arduino
        arduino.write(f"{pan_angle},{tilt_angle}\n".encode())
        print(f"Scanning: {pan_angle},{tilt_angle}")
        time.sleep(2)  # Delay to avoid jitter from spamming random movements

    # ------------- DISPLAY CAMERA FEED WITH TRACKING OVERLAY -------------
    cv2.imshow("Tracker", frame)
    if cv2.waitKey(1) & 0xFF == 27:  # Press ESC to exit
        break

# ------------------ SAVE OFFSET DATA ------------------
with open("offsets.txt", "w") as f:
    f.write(f"{PAN_OFFSET}\n{TILT_OFFSET}\n")

# ------------------ CLEANUP ------------------
cap.release()
cv2.destroyAllWindows()
arduino.close()
