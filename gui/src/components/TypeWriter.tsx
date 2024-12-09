import React, { useState, useEffect, useCallback } from "react";
interface TypingWriterProps {
  text: string;
  status: "doing" | "rollback" | "done";
  className?: string;
}

const TypingWriter = (props: TypingWriterProps) => {
  const { text, status, className } = props;
  const [displayedText, setDisplayedText] = useState("");
  const [index, setIndex] = useState(0);

  const updateText = useCallback(() => {
    if (status === "doing" && index < text.length) {
      setDisplayedText(prev => prev + text[index]);
      setIndex(prev => prev + 1);
    } else if (status === "rollback" && index > 0) {
      setDisplayedText(prev => prev.slice(0, -1));
      setIndex(prev => prev - 1);
    }
  }, [index, text, status]);

  useEffect(() => {
    const timeoutId = setTimeout(updateText, 10);
    return () => clearTimeout(timeoutId);
  }, [updateText]);

  return (
    <div
      className={`text-sm text-gray-500 block whitespace-pre-wrap break-all select-none ${className}`}>
      {displayedText}
    </div>
  );
};

export default TypingWriter;
