import { DarkMode } from '@/types'
import React, { useState, useEffect, useCallback, useMemo } from 'react'
import { IconMoon, IconProps, IconSun } from '@douyinfe/semi-icons'

function getDarkMode(): DarkMode {
  if (!document.body.hasAttribute('theme-mode')) {
    return DarkMode.Light
  }
  if (document.body.getAttribute('theme-mode') === 'dark') {
    return DarkMode.Dark
  }
  // 此时theme-mode不符合预期，需要重置
  document.body.removeAttribute('theme-mode')
  return DarkMode.Light
}

export function useDarkMode() {
  const [mode, setMode] = useState<DarkMode>(getDarkMode())

  const toggleMode = useCallback(() => {
    const body = document.body
    if (body.hasAttribute('theme-mode')) {
      body.removeAttribute('theme-mode')
    } else {
      body.setAttribute('theme-mode', 'dark')
    }
    setMode(getDarkMode())
  }, [])

  useEffect(() => {
    setMode(getDarkMode())
  }, [])

  const IconDarkMode = useMemo(
    () =>
      (
        props: Omit<Omit<IconProps, 'svg' | 'type'>, 'ref'> & React.RefAttributes<HTMLSpanElement>,
      ) => {
        switch (mode) {
          case DarkMode.Light:
            return <IconSun {...props} />
          case DarkMode.Dark:
            return <IconMoon {...props} />
          default:
            return null
        }
      },
    [mode],
  )

  return {
    mode,
    toggleMode,
    IconDarkMode,
  }
}
