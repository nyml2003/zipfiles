import { createRoot } from "react-dom/client";
import App from "@useEntry";
import "./styles.less";
import React from "react";
const container = document.getElementById("root");
const root = createRoot(container as HTMLElement);

root.render(<App />);
