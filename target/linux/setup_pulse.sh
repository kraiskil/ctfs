# Create dummy sink to which the individual processes can
# direct their output
pactl load-module module-null-sink sink_name=treefrogs sink_properties=device.description="treefrogs"
# Connect a copy of the above device into the system default output.
# I.e. play it on speakers.
pactl load-module module-loopback source=treefrogs.monitor
