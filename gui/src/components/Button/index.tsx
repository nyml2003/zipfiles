import React from "react";

type Variant = "confirm" | "primary" | "warning" | "danger" | "success";

interface ButtonProps extends React.ButtonHTMLAttributes<HTMLButtonElement> {
  variant?: Variant;
  visible?: boolean;
}

const Button = React.forwardRef<HTMLButtonElement, ButtonProps>(
  ({ className, variant = "primary", visible, ...props }, ref) => {
    const baseStyles = "px-4 py-2 rounded focus:outline-none";
    const variantStyles: Record<Variant, string> = {
      confirm: "bg-blue-500 text-white hover:bg-blue-600 focus:ring-blue-300 disabled:bg-blue-200",
      primary: "bg-white text-black hover:bg-gray-50 focus:ring-gray-300 disabled:bg-gray-200",
      warning:
        "bg-yellow-500 text-white hover:bg-yellow-600 focus:ring-yellow-300 disabled:bg-yellow-200",
      danger: "bg-red-500 text-white hover:bg-red-600 focus:ring-red-300 disabled:bg-red-200",
      success:
        "bg-green-500 text-white hover:bg-green-600 focus:ring-green-300 disabled:bg-green-200",
    };

    return visible === false ? null : (
      <button
        ref={ref}
        className={`${baseStyles} ${variantStyles[variant]} ${className}`}
        {...props}
      />
    );
  },
);

Button.displayName = "Button";

export default Button;
